#include "Adventure.h"
#include <iostream>

// ===================== GIVEN HELPERS (do not change) =====================

// Reads the next line that is not blank.
// Spaces at the start of the line are removed.
// Returns false when the file has no more lines.
bool Adventure::nextLine(std::ifstream& in, std::string& line) {
    std::string raw;
    while (std::getline(in, raw)) {
        int start = 0;
        while (start < (int)raw.size() && (raw[start] == ' ' || raw[start] == '\t')) {
            start++;
        }
        line = raw.substr(start);
        // Files saved on Windows can end each line with an extra '\r'.
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line = line.substr(0, line.size() - 1);
        }
        if (!line.empty()) {
            return true;
        }
    }
    return false;
}

// Reads the next line and checks that it starts with keyword.
// Example: for the line "HP -2" and keyword "HP", value becomes "-2".
// Prints an error and returns false if the line is missing or wrong.
bool Adventure::readField(std::ifstream& in, const std::string& keyword, std::string& value) {
    std::string line;
    if (!nextLine(in, line)) {
        std::cout << "Error: expected a " << keyword << " line but the file ended." << std::endl;
        return false;
    }
    std::string prefix = keyword + " ";
    if (line.substr(0, prefix.size()) != prefix) {
        std::cout << "Error: expected a " << keyword << " line but found: " << line << std::endl;
        return false;
    }
    value = line.substr(prefix.size());
    return true;
}

// Adds one choice number to a path.
// extendPath("", 2) gives "2". extendPath("2-1", 3) gives "2-1-3".
std::string Adventure::extendPath(const std::string& path, int choiceNumber) const {
    if (path.empty()) {
        return std::to_string(choiceNumber);
    }
    return path + "-" + std::to_string(choiceNumber);
}

// ===================== LOADING =====================

// TODO 10: open the story file and build the tree.
//   - If the file will not open, print exactly:
//       Error: could not open file <fileName>
//     and return false.
//   - Read the START_HP line and save the number in startHp_.
//     (std::stoi turns "10" into 10.)
//   - Build the whole story with readScene. The first scene's label is "START".
//   - If anything failed, return false. Otherwise give the story to tree_.
bool Adventure::load(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Error: could not open file " << fileName << std::endl;
        return false;
    }

    std::string value;
    if (!readField(file, "START_HP", value)) {
        return false;
    }

    startHp_ = std::stoi(value);

    Scene<Moment>* root = readScene(file, "START");
    if (root == nullptr) {
        return false;
    }
    tree_.setRoot(root);
    return true;
}

// TODO 11: read ONE scene and everything below it. Return it.
//   - Read SCENE, HP, LOOT, ITEM, and CHOICES with readField, in that order.
//   - If CHOICES is below 0 or above Scene<Moment>::MAX_CHOICES, print exactly:
//       Error: scene "<scene text>" has <n> choices. The limit is 3.
//     and return nullptr.
//   - Each choice is a LABEL line followed by a whole scene.
//   - If anything goes wrong at any depth, return nullptr
//     and make sure nothing you already built is leaked.
Scene<Moment>* Adventure::readScene(std::ifstream& in, const std::string& label) {
    std::string line;
    std::string childlabel;

    if (!readField(in, "SCENE", line)) {
        return nullptr;
    }

    std::string scene = line;

    if (!readField(in, "HP", line)) {
        return nullptr;
    }

    int hp = std::stoi(line);

    if (!readField(in, "LOOT", line)) {
        return nullptr;
    }

    int loot = std::stoi(line);

    if (!readField(in, "ITEM", line)) {
        return nullptr;
    }

    std::string item = line;

    if (!readField(in, "CHOICES", line)) {
        return nullptr;
    }

    int choice = std::stoi(line);

    if (choice < 0 || choice > Scene<Moment>::MAX_CHOICES) {
        std::cout << "Error: scene \"" << scene << "\" has " << choice << " choices. The limit is " << Scene<Moment>::MAX_CHOICES << "." << std::endl;
        return nullptr;
    }

    Moment* data = new Moment(scene, label, hp, loot, item);
    Scene<Moment>* newScene = new Scene<Moment>(data, choice);

    for (int i = 0; i < choice; i++) {


        if (!readField(in, "LABEL", childlabel)) {
            delete newScene;
            return nullptr;
        }

        Scene<Moment>* child = readScene(in, childlabel);

        if (child == nullptr) {
            delete newScene;
            return nullptr;
        }
        newScene->setChoice(i, child);
    }

    return newScene;
}

// ===================== PLAYING =====================

// TODO 12: ask the player to pick a choice from 1 to numChoices.
//   - Print "Your choice: " (no newline) and read a whole line.
//   - If the line is not a valid number in range, print exactly:
//       Please type a number from 1 to <numChoices>.
//     and ask again.
//   - If the input ends (getline fails), return 0.
int Adventure::askChoice(int numChoices) {
    while (true) {
        std::cout << "Your choice: ";
        std::string choice;
        std::size_t position = 0;

        if (!std::getline(std::cin, choice)) {
            return 0;
        }

        try {
            int num = std::stoi(choice, &position);
            if (position != choice.size() || num < 1 || num > numChoices) {
                std::cout << "Please type a number from 1 to " << numChoices << "." << std::endl;
                continue;
            }
            return num;
        }
        catch (const std::exception& e) {
            std::cout << "Please type a number from 1 to " << numChoices << "." << std::endl;
            continue;
        }
    }
    return 0;
}

// TODO 13: play the story once from the start.
// Every time the player ENTERS a scene, in this order:
//   1. Add its HP change to hp and its loot to loot.
//   2. Print a line of 40 dashes, then the scene text.
//   3. If the scene has an item:
//        if the bag already has it, print "You already have: <item>"
//        otherwise add it to the bag and print "You picked up: <item>"
//   4. Print "HP: <hp>  Loot: <loot>" (two spaces before Loot).
//   5. If hp is 0 or less, print "Your HP hit 0. GAME OVER." and stop.
//      If the scene is an ending, print "THE END." and stop.
//   6. Print each choice as "[1] <label>" and use askChoice.
//      If askChoice returns 0, leave play right away.
// When the game stops, print:
//   Final HP: <hp>  Final loot: <loot>
//   Inventory (<count> items):
// and then print the bag.
// The bag is a List<Item>. Start with HP = startHp_ and loot = 0.
void Adventure::play() {
    int HP = startHp_;
    int loot = 0;
    List<Item> bag;
    Scene<Moment>* current =tree_.getRoot();

    while (current != nullptr) {
        HP+=current->getData()->getHpChange();
        loot+=current->getData()->getLoot();
        std::cout << "----------------------------------------" << std::endl;
        std::cout << current->getData()->getText() << std::endl;

        if (current->getData()->hasItem()) {
            std::string name = current->getData()->getItem();
            Item item(name);
            if (bag.search(item)) {
                std::cout << "You already have: " << name << std::endl;
            }
            else {
                bag.addFront(new Item(name));
                std::cout << "You picked up: " << name << std::endl;

            }
        }

        std::cout << "HP: " << HP << "  Loot: " << loot << std::endl;

        if (HP <= 0) {
            std::cout << "Your HP hit 0. GAME OVER." << std::endl;
            break;
        }
        if (current->isEnding()) {
            std::cout << "THE END." << std::endl;
            break;
        }

        for (int i = 0; i < current->getNumChoices(); i++) {
            Scene<Moment>* child = current->getChoice(i);
            std::cout << "[" << i+1 << "] " << child->getData()->getLabel() << std::endl;
        }

        int selected = askChoice(current->getNumChoices());
        if (selected == 0) {
            break;
        }
        current = current->getChoice(selected-1);
    }


    std::cout << "Final HP: " << HP << "  Final loot: " << loot << std::endl;
    std::cout << "Inventory (" << bag.size() << " items):" << std::endl;
    bag.print();
}

// ===================== ANALYZING =====================

// TODO 14: print exactly three lines:
//   Scenes: <count>
//   Endings: <count>
//   Longest playthrough: <height> choices
void Adventure::printStats() const {
    std::cout << "Scenes: " << tree_.countScenes() << '\n';
    std::cout << "Endings: " << tree_.countEndings() << '\n';
    std::cout << "Longest playthrough: " << tree_.height() << " choices" << std::endl;
}

// TODO 15: print every ending in the story, one per line.
void Adventure::printSpoilers() const {
    printEndings(tree_.getRoot(), "");

}



// TODO 16: print every ending below scene.
// path holds the choice numbers taken to reach scene, like "2-1".
// Each ending prints as:
//   Path <path>: <scene text>
// If the very first scene is an ending, print "Path (start): <scene text>".
// Endings print in order: choice 1 before choice 2 before choice 3.
void Adventure::printEndings(Scene<Moment>* scene, const std::string& path) const {
    if (scene == nullptr) {
        return;
    }
    if (scene->isEnding()) {
        if (path.empty()) {
            std::cout << "Path (start): " << scene->getData()->getText() << std::endl;
        }
        else {
            std::cout << "Path " << path << ": " << scene->getData()->getText() << std::endl;
        }
        return;
    }
    else {
        for (int i = 0; i < scene->getNumChoices(); i++) {
            printEndings(scene->getChoice(i), extendPath(path, i + 1));
        }
    }
}

// TODO 17: the greedy advisor.
// Start at the first scene with startHp_ and 0 loot.
// Enter scenes the same way play does (add HP and loot).
// Stop when hp is 0 or less, or you reach an ending.
// Otherwise always move to the choice with the MOST loot right now.
// On a tie, pick the lower choice number.
// Print:
//   Greedy advisor
//     Path: <path>
//     Loot: <loot>
//     Result: died               (or "Result: reached an ending")
void Adventure::greedyRun() const {
    int HP = startHp_;
    int loot = 0;
    Scene<Moment>* current = tree_.getRoot();
    std::string path = "";

    while (current != nullptr) {
        HP+=current->getData()->getHpChange();
        loot+=current->getData()->getLoot();

        if (HP <= 0) {
            std::cout << "Greedy advisor" << '\n' << "  Path: " << path << '\n' << "  Loot: " << loot << '\n' << "  Result: died" <<std::endl;
            break;
        }
        if (current->isEnding()) {
            std::cout << "Greedy advisor" << '\n' << "  Path: " << path << '\n' << "  Loot: " << loot << '\n' << "  Result: reached an ending" <<std::endl;
            break;
        }

        int highestIndex = 0;

        for (int i = 0; i < current->getNumChoices(); i++) {
            if (current->getChoice(i)->getData()->getLoot() > current->getChoice(highestIndex)->getData()->getLoot()) {
                highestIndex = i;
            }
        }
        path = extendPath(path, highestIndex + 1);
        current = current->getChoice(highestIndex);
    }
}

// TODO 18: the best total loot you can collect from scene down to an ending,
// arriving at scene with hp health and having taken path to get here.
//   - Return -1 if every way down from here kills you.
//   - Put the path of the best run into bestPath.
//   - Must be recursive and look at every choice.
int Adventure::bestLoot(Scene<Moment>* scene, int hp, const std::string& path, std::string& bestPath) const {
    if (scene == nullptr) {
        return -1;
    }
    hp+=scene->getData()->getHpChange();

    if (hp <= 0) {
        return -1;
    }
    if (scene->isEnding()) {
        bestPath = path;
        return scene->getData()->getLoot();
    }

    int best = -1;

    for (int b=0; b < scene->getNumChoices(); b++) {
        Scene<Moment>* child = scene->getChoice(b);
        std::string childPath;
        int childloot = bestLoot(child, hp, extendPath(path, b + 1), childPath);
        if (childloot != -1 && childloot > best) {
            best = childloot;
            bestPath = childPath;
        }
    }
    if (best==-1) {
        return -1;
    }

    return best + scene->getData()->getLoot();
}

// TODO 19: the optimal advisor. Use bestLoot from the first scene.
// Print:
//   Optimal advisor
//     Path: <path>
//     Loot: <loot>
//     Result: reached an ending
// If no path survives, print only:
//   Optimal advisor
//     Result: no path survives this story
void Adventure::optimalRun() const {
    std::string path = "";
    int loot = bestLoot(tree_.getRoot(), startHp_, "", path);
    if (loot == -1) {
        std::cout << "Optimal advisor" << '\n' << "  Result: no path survives this story" << std::endl;
    }
    else {
        std::cout << "Optimal advisor" << '\n' << "  Path: " << path << '\n' << "  Loot: " << loot << '\n' << "  Result: reached an ending" << std::endl;
    }
}
