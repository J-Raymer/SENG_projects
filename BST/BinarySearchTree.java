import java.util.*;

//
// An implementation of a binary search tree.
//
// This tree stores both keys and values associated with those keys.
//
// More information about binary search trees can be found here:
//
// http://en.wikipedia.org/wiki/Binary_search_tree
//
// Note: Wikipedia is using a different definition of
//       depth and height than we are using.  Be sure
//       to read the comments in this file for the
//	 	 height function.
//
class BinarySearchTree <K extends Comparable<K>, V>  {

	public static final int BST_PREORDER  = 1;
	public static final int BST_POSTORDER = 2;
	public static final int BST_INORDER   = 3;

	// These are package friendly for the TreeView class
	BSTNode<K,V>	root;
	int		count;
	int		findLoops;
	int		insertLoops;

	public BinarySearchTree () {
		root = null;
		count = 0;
		resetFindLoops();
		resetInsertLoops();
	}

	public int getFindLoopCount() {
		return findLoops;
	}

	public int getInsertLoopCount() {
		return insertLoops;
	}

	public void resetFindLoops() {
		findLoops = 0;
	}
	public void resetInsertLoops() {
		insertLoops = 0;
	}

	//
	// Purpose:
	//
	// Insert a new Key:Value Entry into the tree.  If the Key
	// already exists in the tree, update the value stored at
	// that node with the new value.
	//
	// Pre-Conditions:
	// 	the tree is a valid binary search tree
	//
	public void insert (K k, V v) {
		BSTNode<K,V> newNode = new BSTNode(k,v);
		if(this.count == 0){
			root = newNode;
			this.count++;
		}
		searchAndAdd(this.root, newNode);
		
	}

	//
	// Purpose:
	//
	// Return the value stored at key.  Throw a KeyNotFoundException
	// if the key isn't in the tree.
	//
	// Pre-conditions:
	//	the tree is a valid binary search tree
	//
	// Returns:
	//	the value stored at key
	//
	// Throws:
	//	KeyNotFoundException if key isn't in the tree
	//
	public V find (K key) throws KeyNotFoundException {
		if(this.count == 0){
		throw new KeyNotFoundException();
		}
		// try to get data from key recursively and return it if found
		V keyValue = getDataFromKey(this.root, key);
		if (keyValue != null){
			return keyValue;
		}
		//if KeyValue isnt returned then throw KeyNotFoundExeption
		throw new KeyNotFoundException();
	}

	//
	// Purpose:
	//
	// Return the number of nodes in the tree.
	//
	// Returns:
	//	the number of nodes in the tree.
	public int size() {
		return this.count;
	}

	//
	// Purpose:
	//	Remove all nodes from the tree.
	//
	public void clear() {
		root = null;
		count = 0;
		resetFindLoops();
		resetInsertLoops();
	}

	//
	// Purpose:
	//
	// Return the height of the tree.  We define height
	// as being the number of nodes on the path from the root
	// to the deepest node.
	//
	// This means that a tree with one node has height 1.
	//
	// Examples:
	//	See the assignment PDF and the test program for
	//	examples of height.
	//
	public int height() {
		if(this.count == 0){
			return 0;
		}
		// if count isn't 0 find height recursively
		return findHeight(this.root);
	}

	//
	// Purpose:
	//
	// Return a list of all the key/value Entrys stored in the tree
	// The list will be constructed by performing a level-order
	// traversal of the tree.
	//
	// Level order is most commonly implemented using a queue of nodes.
	//
	//  From wikipedia (they call it breadth-first), the algorithm for level order is:
	//
	//	levelorder()
	//		q = empty queue
	//		q.enqueue(root)
	//		while not q.empty do
	//			node := q.dequeue()
	//			visit(node)
	//			if node.left != null then
	//			      q.enqueue(node.left)
	//			if node.right != null then
	//			      q.enqueue(node.right)
	//
	// Note that we will use the Java LinkedList as a Queue by using
	// only the removeFirst() and addLast() methods.
	//
	public List<Entry<K,V>> entryList() {
		List<Entry<K, V>> l = new LinkedList<Entry<K,V> >();
		// queue of keys to keep track of which ones have been visited
		LinkedList<BSTNode> visitedNodes = new LinkedList<BSTNode>();
		
		visitedNodes.addLast(this.root);
		// while not q.empty do
		while(!visitedNodes.isEmpty()){
			//node := q.dequeue()
			BSTNode currNode = visitedNodes.removeFirst();
			//visit(node)
			l.add(new Entry(currNode.key, currNode.value));
			//if node.left != null then
			if(currNode.left != null){
				//q.enqueue(node.left)
				visitedNodes.addLast(currNode.left);
			}
			//if node.right != null then
			if(currNode.right != null){
				//q.enqueue(node.right)
				visitedNodes.addLast(currNode.right);
			}
			
		}
		return l;
	}

	//
	// Purpose:
	//
	// Return a list of all the key/value Entrys stored in the tree
	// The list will be constructed by performing a traversal 
	// specified by the parameter which.
	//
	// If which is:
	//	BST_PREORDER	perform a pre-order traversal
	//	BST_POSTORDER	perform a post-order traversal
	//	BST_INORDER	perform an in-order traversal
	//
	public List<Entry<K,V> > entryList (int which) {
		List<Entry<K,V> > l = new LinkedList<Entry<K,V> >();
		
		if (this.count == 0 ) return l;

		switch(which){
			case BST_PREORDER: 
				this.preOrder(this.root, l);
				break;
			case BST_POSTORDER: 
				this.postOrder(this.root, l);
				break;
			case BST_INORDER: 
				this.inOrder(this.root, l);
				break;
			default:// default to inOrder if incorrect input is used
				this.inOrder(this.root, l);
		}		
		return l;
	}

	//
	// My private methods START
	//
	
	private int findHeight(BSTNode node){
		//base case (reached leaf of tree)
		if(node == null) return 0;
		//find what side is larger (left or right) then add the number of nodes plus the root node
		if(findHeight(node.left) >= findHeight(node.right)){
			return (1 + findHeight(node.left));
		}
		else{
			return (1 + findHeight(node.right));
		}
		
	}
	
	private V getDataFromKey(BSTNode<K,V> currNode, K key){
		this.findLoops++;
		// if the key we are trying to find is greater than currNode go search right
		if(currNode.key.compareTo(key) < 0){
			// if the right node isnt empty keep searching
			if(currNode.right != null){
				return getDataFromKey(currNode.right, key);
			}
		}
		// if the key we are trying to find is less than currNode search left
		else if (currNode.key.compareTo(key) > 0){
			// if the left node isnt empty keep searching
			if(currNode.left !=null){
				return getDataFromKey(currNode.left, key);
			}
		}
		else{
			return currNode.value;
		}
		return null;
	}
	
	private void searchAndAdd(BSTNode<K,V> currNode, BSTNode<K,V> newNode){
		this.insertLoops++;
		// if newNode is greater than currNode try to insert it to the right
		if(currNode.key.compareTo(newNode.key) < 0){
			if(currNode.right == null){
				currNode.right = newNode;
				this.count++;
				return;
			}
			//if right node not empty keep searching
			searchAndAdd(currNode.right, newNode);
		}
		//else if newNode is less than currNode try to insert it to the left
		else if (currNode.key.compareTo(newNode.key) > 0){
			if(currNode.left == null){
				currNode.left = newNode;
				this.count++;
				return;
			}
			//if left node not empty keep searching
			searchAndAdd(currNode.left, newNode);			
		}
		// else the key is the same, update value
		else{
			currNode.value = newNode.value;
			return;
		}
	}
	
	
	// Your instructor had the following private methods in his solution:
	// private void doInOrder (BSTNode<K,V> n, List <Entry<K,V> > l);
	// private void doPreOrder (BSTNode<K,V> n, List <Entry<K,V> > l);
	// private void doPostOrder (BSTNode<K,V> n, List <Entry<K,V> > l);
	// private int doHeight (BSTNode<K,V> t)
	
	private void inOrder (BSTNode<K,V> n, List <Entry<K,V> > l){
		// Base case 
		if (n == null) return; 
		inOrder(n.left, l);
		l.add(new Entry(n.key, n.value));
		inOrder(n.right, l);
	}

	private void preOrder (BSTNode<K,V> n, List <Entry<K,V> > l){
		//Base case
		if (n == null) return;
		l.add(new Entry(n.key, n.value));
		preOrder(n.left, l);
		preOrder(n.right, l);
	}

	private void postOrder (BSTNode<K,V> n, List <Entry<K,V> > l){
		//Base case
		if (n == null) return; 		
		postOrder(n.left, l);
		postOrder(n.right, l);
		l.add(new Entry(n.key, n.value));
	}
}
	
	//
	// My private methods END
	//
	

