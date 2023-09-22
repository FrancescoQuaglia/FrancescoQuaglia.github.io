# SINGLE File FS

This FS contains a single file, the block organization is the following.

```
-----------------------------------------------------
| Block 0    | Block 1      | Block 2    | Block 3   |
| Superblock | inode of the | File data  | File data |
|            | File         |            |           |
-----------------------------------------------------
```
The root directory is volatile, its inode does not exist on disk, it is simly created upon file system mount.

Current operations:
- iterate, used to read a directory, it reads the information of the root directory, which has only one children, the single file
- lookup, connect a dentry to an inode (this is used by ls to read the file information)
- file read, reads our unique file

There is also a simple makefs script to format a device for this filesystem.

Current information created by makefs:
- The super block information
- File inode
- File contents

The FS will not not accept a mount on a disk that is not correctly set up with the superblock information. 


