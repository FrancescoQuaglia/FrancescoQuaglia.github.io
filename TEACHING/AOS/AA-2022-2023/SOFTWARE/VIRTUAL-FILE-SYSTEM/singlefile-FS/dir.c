#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/timekeeping.h>
#include <linux/time.h>
#include <linux/buffer_head.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/string.h>

#include "singlefilefs.h"

//this iterate function just returns 3 entries: . and .. and then the name of the unique file of the file system
static int onefilefs_iterate(struct file *file, struct dir_context* ctx) {

    //	printk("%s: we are inside readdir with ctx->pos set to %lld", MOD_NAME, ctx->pos);
	
	if(ctx->pos >= (2 + 1)) return 0;//we cannot return more than . and .. and the unique file entry

	if (ctx->pos == 0){
 //   		printk("%s: we are inside readdir with ctx->pos set to %lld", MOD_NAME, ctx->pos);
		if(!dir_emit(ctx,".", FILENAME_MAXLEN, SINGLEFILEFS_ROOT_INODE_NUMBER, DT_UNKNOWN)){
			return 0;
		}
		else{
			ctx->pos++;
		}
	
	}

	if (ctx->pos == 1){
  //  		printk("%s: we are inside readdir with ctx->pos set to %lld", MOD_NAME, ctx->pos);
		//here the inode number does not care
		if(!dir_emit(ctx,"..", FILENAME_MAXLEN, 1, DT_UNKNOWN)){
			return 0;
		}
		else{
			ctx->pos++;
		}
	
	}
	if (ctx->pos == 2){
   // 		printk("%s: we are inside readdir with ctx->pos set to %lld", MOD_NAME, ctx->pos);
		if(!dir_emit(ctx, UNIQUE_FILE_NAME, FILENAME_MAXLEN, SINGLEFILEFS_FILE_INODE_NUMBER, DT_UNKNOWN)){
			return 0;
		}
		else{
			ctx->pos++;
		}
	
	}

	return 0;

}

//add the iterate function in the dir operations
const struct file_operations onefilefs_dir_operations = {
    .owner = THIS_MODULE,
    .iterate = onefilefs_iterate,
};
