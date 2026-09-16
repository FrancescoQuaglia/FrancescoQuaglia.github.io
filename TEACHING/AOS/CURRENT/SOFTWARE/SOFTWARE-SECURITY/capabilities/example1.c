#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/capability.h>
#include <errno.h>
#include <linux/securebits.h>

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void)
{
    printf("[+] Starting program as UID=%d, EUID=%d\n",
           getuid(), geteuid());

    if (geteuid() != 0) {
        fprintf(stderr, "[-] Must be run as root\n");
        return EXIT_FAILURE;
    }

    /* -------------------------------------------------
     * Step 1: Explicitly load ALL capabilities
     * ------------------------------------------------- */
    cap_t caps = cap_init();
    if (!caps)
        die("cap_init");

    int last = CAP_LAST_CAP;
    cap_value_t cap_list[CAP_LAST_CAP + 1];

    for (int i = 0; i <= last; i++)
        cap_list[i] = i;

    if (cap_set_flag(caps, CAP_PERMITTED,
                     last + 1, cap_list, CAP_SET) < 0)
        die("cap_set_flag(PERMITTED)");

    if (cap_set_flag(caps, CAP_EFFECTIVE,
                     last + 1, cap_list, CAP_SET) < 0)
        die("cap_set_flag(EFFECTIVE)");

    if (cap_set_proc(caps) < 0)
        die("cap_set_proc");

    cap_free(caps);

    printf("[+] All capabilities loaded into permitted+effective\n");

    /* -------------------------------------------------
     * Step 2: Keep capabilities after dropping UID
     * ------------------------------------------------- */
    if (prctl(PR_SET_SECUREBITS, SECBIT_KEEP_CAPS) < 0)
        die("prctl(PR_SET_SECUREBITS)");

    printf("[+] SECBIT_KEEP_CAPS set\n");

    /* -------------------------------------------------
     * Step 3: Drop privileges
     * ------------------------------------------------- */
    uid_t target_uid = 1000;   /* must exist on system */
    if (setuid(target_uid) < 0)
        die("setuid");

    printf("[+] Dropped to UID=%d, EUID=%d\n",
           getuid(), geteuid());

    /* -------------------------------------------------
     * Step 4: Verify capabilities
     * ------------------------------------------------- */
    caps = cap_get_proc();
    if (!caps)
        die("cap_get_proc");

    char *txt = cap_to_text(caps, NULL);
    if (!txt)
        die("cap_to_text");

    printf("[+] Current capabilities: %s\n", txt);

    cap_free(txt);
    cap_free(caps);

    /* -------------------------------------------------
     * Step 5: Exec shell
     * ------------------------------------------------- */
    printf("[+] Executing /bin/sh\n");
    execl("/bin/sh", "/bin/sh", NULL);

    die("execl");
}

