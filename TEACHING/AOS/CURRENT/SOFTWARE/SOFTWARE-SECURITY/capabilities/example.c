#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <linux/securebits.h>
#include <sys/capability.h>
#include <errno.h>

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    printf("[+] Starting program as UID=%d, EUID=%d\n", getuid(), geteuid());

    //baseline check
    if(geteuid() != 0){
        die("[-] not running with root capability - just exiting\n");
    }

    // Step 1: Set SECBIT_KEEP_CAPS using prctl
    if (prctl(PR_SET_SECUREBITS, SECBIT_KEEP_CAPS) < 0) {
        die("[-] Failed to set SECBIT_KEEP_CAPS");
    }
    printf("[+] SECBIT_KEEP_CAPS set successfully!\n");

    // Step 2: Drop privileges to non-root UID (e.g., UID=1000)
    uid_t target_uid = 1000;  // Change to a valid, existing non-root UID
    if (setuid(target_uid) < 0) {
        die("[-] Failed to drop privileges with setuid");
    }
    printf("[+] Privileges dropped to UID=%d, EUID=%d\n", getuid(), geteuid());

    // Step 3: Verify capabilities
    cap_t caps = cap_get_proc();  // Get current capabilities
    if (caps == NULL) {
        die("[-] Failed to get capabilities");
    }

    printf("[+] Current capabilities: %s\n", cap_to_text(caps, NULL));
    cap_free(caps);

    // Step 4: Run a shell
    printf("[+] Running /bin/sh...\n");
    execl("/bin/sh", "/bin/sh", NULL);

    // If execl() fails
    die("[-] Failed to execute shell");

    return 0;
}

