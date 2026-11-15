1. For every function you wrote (TODO 1 to 19), explain in your own words what it does and
     give its time complexity in terms of n, the number of scenes. One short paragraph each.
* TODO 1: If numChoices exceeds 0, the constructor allocates an array with numChoices_ child pointer slots. The parent scene 
starts at index 0 and goes up to numChoices_-1, and each slot gets initialized to nullptr. Big O: O(1)
* TODO 2: If the scene contains non-nullptr values from index 0 to numChoices_-1, the program frees up the memory by deleting
the objects, the array with child scene pointers, and the data belonging to a current scene. Each child destructor frees the descendants.
Big O: O(n)
* TODO 3: When the player reaches an ending, the program checks if there are any remaining choices. It returns true if
numChoices==0 and false if numChoices is not 0. Big O: O(1)
* TODO 4: If index is outside the valid range, that means index isn't a real slot and returns nullptr. If it is valid,
it returns the scene. Big O: O(1)
* TODO 5: If the index is outside the valid range, it returns unchanged. Otherwise, the program will assign the next scene pointer
to choices_[index]. Invalid indices are ignored. Big O: O(1)
* TODO 6: If there are no scenes, it returns 0. The program initializes with a single scene, then it counts every scene
to calculate the total number of scenes. Big O: O(n)
* TODO 7: If there are no scenes, it returns 0. The program evaluates whether every single node contains any child scenes
by traversing through every path of the tree. Every time the program reaches a scene without children, it counts as an
ending, and returns the total after each count. Big O: O(n)
* TODO 8: A tree with no scenes has height initialized to -1, and returns height 0 when it reaches an ending to indicate
there are no children below it. The height of a non-ending scene is 1 plus the child's greatest height. Using the for loop, 
the program will traverse through every single child scene and if the child height exceeds the maximum, then max height is 
replaced with child height. Big O: O(n)
* TODO 9: When emptying the tree, the program has to delete every scene including the root, leaving with root as nullptr. Big O: O(n)
* TODO 10: The starting HP is stored in startHp_. If the file fails to open, it prints the given error message. Otherwise, it 
reads the starting HP value. If the program fails to read the first 'START_HP' line, it returns false, and if it reads correctly,
then the program uses the readScene() function to build the tree and sets up the root using tree_.setRoot(root). Big O: O(n)
* TODO 11: In the game, the program reads each variable. It will return nullptr when the program fails to read them, and it will
return a value if the program reads the variables correctly. If the choice is below 0 or exceeds the max number of choices which is 3, 
it will print an error message and return nullptr. The program provides a label for every choice, which recursively attaches a child, 
and if it fails to read the label, it deletes the attached child scene and returns nullptr instead. Big O: O(n)
* TODO 12: std::getline() reads a line, while std::stoi() converts it. The program will ask the player to type in a choice from the valid
number of choices it provided. When the user inputs an invalid value or an integer outside the valid range, it will reject the input by 
asking the player again until the player meets those requirements. The purpose of the try-catch block is to handle exceptions during 
conversion failures by std::stoi(). Big O: O(1)
* TODO 13: Initially, the program sets up the data for HP, loot, and items, and every time the player enters a new and valid scene, the
variables HP and loot get updated and the program checks for duplicate items in the player's inventory. When the player reaches an 
ending scene, it will print the final HP and final loot and the number of items collected. Play() displays the choices in order to transition
to the next scene, and stops when either the player dies or reaches an ending. Big O: O(n^2)
* TODO 14: The program calls three functions tree_.countScenes(), tree_.countEndings(), and tree_.height() to print the final results. Big O: O(n)
* TODO 15: It calls printEndings(tree_.getRoot(), "") to start the recursive spoiler traversal and prints all the endings out and separately by line.
Big O: O(n)
* TODO 16: Every recursive child call extends the path with extendPath(path, i+1). If the program reached an ending scene, and if the 
path is empty, it'll print the starting path. Otherwise, it'll print the path and the scene text. If the program didn't reach an ending, 
it has to keep traversing. Big O: O(n)
* TODO 17: Initially, HP is set to an integer value. The greedy advisor compares each child's loot in order to choose. If the greedy advisor 
reaches an HP of 0 or below, it ends up dying. If the HP is above 0 while the advisor reached an ending, that means the run was successful. 
The role of the greedy advisor is to select a choice with the highest immediate loot without knowing the consequences. Big O: O(n)
* TODO 18: The program recursively traverses to examine every child subtree and selects the path with the highest loot while rejecting the 
fatal paths. Its target is to return the best total loot. If the player dies, then the program returns -1. If the program reaches a surviving 
ending scene, it saves the path and returns the scene's loot, as for a non-ending scene, it adds the current scene's loot to the player's 
best child result, and returns the player's looting data. Big O: O(n)
* TODO 19: The function bestLoot() is called from the root with an empty path and a set HP. If loot=-1, then no path survives in the program.
Otherwise, it prints the best path, total loot, and the overall result. Big O: O(n)
* 
2. Why does each scene allocate exactly numChoices slots instead of declaring Scene<T>*
   choices[3]? What does an ending hold in choices_, and why?
* If only the 3-pointer slots are allocated for every scene, it takes up a lot of space, while 
allocating a specific number of slots wouldn't take up an unnecessary amount of memory. The ending
holds a nullptr in choices_ because the ending doesn't possess any child scenes and the program will 
avoid allocating unnecessary memory.
* 
3. Walk through your Scene destructor. What gets deleted, and in what order? What would
   go wrong if choices_ were deleted first?
* For every scenes inside the valid range, it will first delete the child scene objects in the array.
Then, it will delete the array that contains all of the child pointers, and finally, it will delete 
the data for the current scene. If choices were deleted first, the array will get deleted first but
objects themselves would still exist in memory which would cause memory leaks and undefined behavior.
* 
4. Show the path greedy took and the path optimal took on your story, with the loot for each.
   Explain why greedy lost. If greedy did not lose, change your story until it does, and explain
   what you changed.
* The path my greedy advisor took is 1-1-2-1 and collected a total of 15 loot, whereas my optimal advisor 
took the path 1-2-2-1-1 and collected a total of 38 loot. My greedy advisor chooses that path in order
to search for immediate loot that each child scene could offer, where it becomes unaware of the future
consequences which could strike the player and affect survival by dramatically reducing its HP down to 0 or below 0.
That's how my greedy advisor ended up dying.
* 
5. bestLoot tries every path, yet it runs in O(n) time. Explain why it is not exponential, using
   what you know about paths and leaves in a tree.
* It is not exponential because bestLoot() visits every scene once and the maximum choices the scene has is
3 choices. Every ending is a leaf, and exploring root-to-leaf paths doesn't need repeated traversals.
* 
6. Which List methods does your game use, and why? What does one search cost as the bag
   grows?
* The list method that my game uses are bag.addFront(), bag.size(), bag.print(), and bag.search(). These 
methods are used in order to keep track of the condition of the bag. Using bag.search(), the program may have to go through 
all the inventory's items in order to determine if items could be added to the bag using bag.addFront()
or if the player collects a duplicate item. The role of bag.size() is to count the number of items contained in the inventory.
As the size of the bag grows with up to n items, the bag has to go through and examine all n items, where 
the worst time complexity becomes O(n). If the program fails to find the target item, it returns false. Using bag.print(), 
the program prints all the collected items.