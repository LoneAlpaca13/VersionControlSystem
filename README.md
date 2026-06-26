# Time-Travelling File System  

A simplified **in-memory version control system** inspired by Git.  
This system allows you to:  
- Create files  
- Modify contents  
- Maintain **version history as a tree**  
- Perform analytics like querying most recent or largest files  

Unlike typical implementations, this project uses **custom-built Trees, HashMaps, and Heaps** instead of standard library equivalents.  

---

## Features  

### Core File Operations  

- **`CREATE <filename>`**  
  Create a new file with:  
  - Initial root version (ID `0`)  
  - Empty content  
  - Snapshot message `"Initial snapshot"`  

- **`READ <filename>`**  
  Display the content of the file’s currently active version.  

- **`INSERT <filename> <content>`**  
  Append content to the file.  
  - If active version is already snapshotted → create a new version.  

- **`UPDATE <filename> <content>`**  
  Replace file’s content.  
  - If active version is snapshotted → create a new version.  
  - Special case: if content is `"RESET"`, file is cleared.  

- **`SNAPSHOT <filename> <message>`**  
  Mark the active version as a snapshot, storing the message and timestamp.  

- **`ROLLBACK <filename> [versionID]`**  
  - Without argument → roll back to parent of current version.  
  - With `versionID` → jump to that version.  
  - Root version cannot be rolled back further.  

- **`HISTORY <filename>`**  
  Print chain of versions from active version back to root, showing:  
  - Version ID  
  - Timestamp  
  - Snapshot message  

---

###  System-Wide Analytics  

- **`RECENT_FILES <num>`**  
  Show the top `<num>` most recently updated files.  

- **`BIGGEST_FILES <num>`**  
  Show the top `<num>` files with the largest number of versions.  

---

##  Internal Data Structures  

### 1. **Tree (Version History)**  
Each file’s version history is stored as a **tree of `TreeNodes`**.  
- `version_id` (int, sequential starting at 0)  
- `content` (string)  
- `message` (snapshot description)  
- `created_timestamp`  
- `snapshot_timestamp` (if snapshotted)  
- Pointers → parent + children  

This enables **branching and rollback functionality**.  

---

### 2. **HashMap (Version Lookup)**  
A custom **open addressing HashMap** (using modulo hashing).  
- Maps `version_id → TreeNode*` (O(1) lookup)  
- Globally maps `filename → File*`  

---

### 3. **Heap (System Analytics)**  
Custom **array-based binary heap** with heapify operations:  
- **`heapifyRecent`** → order files by `last_updated` timestamp  
- **`heapifyBiggest`** → order files by total version count  

---

##  Notes  

- **Timestamps**  
  - Uses `time(nullptr)` for version and snapshot creation.  
  - Multiple operations in the same second may cause ties.  
  - Tied results in analytics may appear in arbitrary order.  

- **Error Handling**  
  - `CREATE` on existing file → error.  
  - Accessing non-existent file → error.  
  - `ROLLBACK` cannot go past root version.  
  - If `RECENT_FILES` or `BIGGEST_FILES` request exceeds available files → error.  
