#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/timekeeping.h>
#include <linux/time.h>
#include <linux/buffer_head.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/version.h>      /* For LINUX_VERSION_CODE */
#include <linux/user_namespace.h>


#include "singlefilefs.h"

ssize_t onefilefs_read(struct file *, char __user *, size_t, loff_t *); 
ssize_t onefilefs_write(struct file *, const char __user *, size_t, loff_t *); 
loff_t onefilefs_seek(struct file *, loff_t, int);
struct dentry *onefilefs_lookup(struct inode *, struct dentry *, unsigned int);

#define BH_Dirty (0x2)


ssize_t onefilefs_read(struct file * filp, char __user * buf, size_t len, loff_t * off) {

    struct buffer_head *bh = NULL;
    struct inode * the_inode = filp->f_inode;
    uint64_t file_size;
    int ret;
    loff_t offset;
    int block_to_read;//index of the block to be read from device

    inode_lock(the_inode);

    file_size = the_inode->i_size;
    printk("%s: read operation called with len %ld - and offset %lld (the current file size is %lld)",MOD_NAME, len, *off, file_size);

    //check that *off is within boundaries
    if (*off >= file_size){
    	inode_unlock(the_inode);
        return 0;
    }
    else if (*off + len > file_size)
        len = file_size - *off;

    //determine the block level offset for the operation
    offset = *off % DEFAULT_BLOCK_SIZE; 
    //just read stuff in a single block - residuals will be managed at the applicatin level
    if (offset + len > DEFAULT_BLOCK_SIZE)
        len = DEFAULT_BLOCK_SIZE - offset;

    //compute the actual index of the the block to be read from device
    block_to_read = *off / DEFAULT_BLOCK_SIZE + 2; //the value 2 accounts for superblock and file-inode on device
    
    printk("%s: read operation must access block %d of the device",MOD_NAME, block_to_read);

    bh = (struct buffer_head *)sb_bread(filp->f_path.dentry->d_inode->i_sb, block_to_read);
    if(!bh){
    	inode_unlock(the_inode);
	return -EIO;
    }
    ret = copy_to_user(buf,bh->b_data + offset, len);
    *off += (len - ret);
    brelse(bh);

    inode_unlock(the_inode);
    return len - ret;

}

ssize_t onefilefs_write(struct file * filp, const char __user * buf, size_t len, loff_t * off) {

    struct onefilefs_inode *FS_specific_inode;
    struct buffer_head *bh = NULL;
    struct inode * the_inode = filp->f_inode;
    uint64_t file_size;
    uint64_t max_file_size;
    struct onefilefs_sb_info *sb_disk;
    int ret;
    loff_t offset;
    int block_to_write;//index of the block to be written from device

    inode_lock(the_inode);
    file_size = the_inode->i_size;
    printk("%s: write operation called with len %ld - and offset %lld (the current file size is %lld)",MOD_NAME, len, *off, file_size);

    bh = sb_bread(filp->f_inode->i_sb, SB_BLOCK_NUMBER);
    if(!bh){
    	inode_lock(the_inode);
        return -EIO;
    }
    sb_disk = (struct onefilefs_sb_info *)bh->b_data;
    max_file_size = sb_disk->max_file_size;
    brelse(bh);

    printk("%s: max file size is %lld",MOD_NAME, max_file_size);

    if(*off > max_file_size){
    	inode_unlock(the_inode);
        return -(ENOMEM);
    }

    //check that *off is within boundaries
    if (*off > file_size){
    	inode_unlock(the_inode);
        return 0;
    }
    else if (*off + len > max_file_size)
        len = max_file_size - *off;


    //determine the block level offset for the operation
    offset = *off % DEFAULT_BLOCK_SIZE;
    //just write stuff in a single block - residuals will be managed at the application level
    if (offset + len > DEFAULT_BLOCK_SIZE)
        len = DEFAULT_BLOCK_SIZE - offset;

    //compute the actual index of the the block to be read from device
    block_to_write = *off / DEFAULT_BLOCK_SIZE + 2; //the value 2 accounts for superblock and file-inode on device

    printk("%s: write operation must access block %d of the device",MOD_NAME, block_to_write);

    bh = (struct buffer_head *)sb_bread(filp->f_path.dentry->d_inode->i_sb, block_to_write);
    if(!bh){
    	inode_unlock(the_inode);
        return -EIO;
    }

    ret = copy_from_user(bh->b_data + offset, buf, len);
    *off += (len - ret);

    //flag update indicating the dirty-ness of the block
    bh->b_state = bh->b_state | BH_Dirty;
    write_dirty_buffer(bh, 0);
    brelse(bh);

    if(the_inode->i_size < *off){
    	the_inode->i_size = *off;
	//update file i-node on device
        bh = (struct buffer_head *)sb_bread(filp->f_inode->i_sb, SINGLEFILEFS_INODES_BLOCK_NUMBER );
        if(!bh){
    		inode_unlock(the_inode);
                return -EIO;
        }
        FS_specific_inode = (struct onefilefs_inode*)bh->b_data;
        FS_specific_inode->file_size = the_inode->i_size;
   	bh->b_state = bh->b_state | BH_Dirty;
        write_dirty_buffer(bh, 0);
        brelse(bh);
    }

    inode_unlock(the_inode);
    return len - ret;

}

loff_t onefilefs_seek(struct file * filp, loff_t off, int command) {
    struct inode * the_inode = filp->f_inode;
    loff_t ret;  

    inode_lock(the_inode);
    if(the_inode->i_size < off || command != SEEK_SET){
	ret = -(EINVAL);
    }
    else{
	filp->f_pos = off;
	ret = off;
    }
    inode_unlock(the_inode);
   
    return ret;
}
	


struct dentry *onefilefs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags) {

    struct onefilefs_inode *FS_specific_inode;
    struct super_block *sb = parent_inode->i_sb;
    struct buffer_head *bh = NULL;
    struct inode *the_inode = NULL;

    printk("%s: running the lookup inode-function for name %s",MOD_NAME,child_dentry->d_name.name);

    if(!strcmp(child_dentry->d_name.name, UNIQUE_FILE_NAME)){

	
	//get a locked inode from the cache 
        the_inode = iget_locked(sb, 1);
        if (!the_inode)
       		 return ERR_PTR(-ENOMEM);

	//already cached inode - simply return successfully
	if(!(the_inode->i_state & I_NEW)){
		unlock_new_inode(the_inode);
		return child_dentry;
	}


	//this work is done if the inode was not already cached
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 3, 0)
#error "Kernel versions older than 6.3.0 are no longer supported."
#else
	inode_init_owner(&nop_mnt_idmap,the_inode, NULL, S_IFREG );
#endif
	the_inode->i_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH;
        the_inode->i_fop = &onefilefs_file_operations;
	the_inode->i_op = &onefilefs_inode_ops;

	//just one link for this file
	set_nlink(the_inode,1);

	//now we retrieve the file size via the FS specific inode, putting it into the generic inode
    	bh = (struct buffer_head *)sb_bread(sb, SINGLEFILEFS_INODES_BLOCK_NUMBER );
    	if(!bh){
		iput(the_inode);
		return ERR_PTR(-EIO);
    	}
	FS_specific_inode = (struct onefilefs_inode*)bh->b_data;
	the_inode->i_size = FS_specific_inode->file_size;
        brelse(bh);

    	printk("%s: file lookup operation - file size is %lld",MOD_NAME, the_inode->i_size);

        d_add(child_dentry, the_inode);
	dget(child_dentry);

	//unlock the inode to make it usable 
    	unlock_new_inode(the_inode);

	return child_dentry;
    }

    return NULL;

}

//look up goes in the inode operations
const struct inode_operations onefilefs_inode_ops = {
    .lookup = onefilefs_lookup,
};

const struct file_operations onefilefs_file_operations = {
    .owner = THIS_MODULE,
    .llseek = onefilefs_seek,
    .read = onefilefs_read,
    .write = onefilefs_write,
    //.write = onefilefs_write //please implement this function to complete the exercise
};
