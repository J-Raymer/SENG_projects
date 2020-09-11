import java.util.*;

public class BSTMap<K extends Comparable<K>, V > implements  Map<K, V>  {
	
	BinarySearchTree<K, V> BSTMap;

	public BSTMap () {
		this.BSTMap = new BinarySearchTree<K, V>();
	}

	public boolean containsKey(K key) {
		boolean hasEntry = false;
		try{
			V value = this.BSTMap.find(key);
			Entry<K, V> entry = new Entry(key, value);
			List<Entry<K, V>> entries = this.BSTMap.entryList();
			hasEntry = entries.contains(entry);
		} catch (KeyNotFoundException e){}
		return hasEntry;
	}

	public V get (K key) throws KeyNotFoundException {
		return this.BSTMap.find(key);
		
	}

	public List<Entry<K,V> >	entryList() {
		return this.BSTMap.entryList();
	}

	public void put (K key, V value) {
		this.BSTMap.insert(key, value);
	}

	public int size() {
		return this.BSTMap.size();
	}

	public void clear() {
		this.BSTMap.clear();
	}

	public int getGetLoopCount() {
		return this.BSTMap.getFindLoopCount();
	}

	public int getPutLoopCount() {
		return this.BSTMap.getInsertLoopCount();
	}

	public void resetGetLoops() {
		this.BSTMap.resetFindLoops();
	}
	public void resetPutLoops() {
		this.BSTMap.resetInsertLoops();
	}
}
