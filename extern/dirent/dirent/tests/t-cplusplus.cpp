/*
 * Test program to make sure that dirent compiles cleanly with C++
 *
 * Copyright (C) 2006-2012 Toni Ronkko
 * This file is part of dirent.  Dirent may be freely distributed
 * under the MIT license.  For all details and documentation, see
 * https://github.com/tronkko/dirent
 */
#include <iostream>
#include <string.h>
#include <dirent.h>
#include <assert.h>
using namespace std;

int
main(
    int argc, char *argv[]) 
{
    (void) argc;
    (void) argv;

    /* Basic directory retrieval */
    {
        DIR *dir;
        struct dirent *ent;
        int found = 0;

        /* Open directory */
        dir = opendir ("tests/1");
        if (dir == NULL) {
            cerr << "Directory tests/1 not found" << endl;
            abort ();
        }

        /* Read entries */
        while ((ent = readdir (dir)) != NULL) {

            /* Check each file */
            if (strcmp (ent->d_name, ".") == 0) {
                /* Directory itself */
#ifdef _DIRENT_HAVE_D_TYPE
                assert (ent->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
                assert (ent->d_namlen == 1);
#endif
#ifdef _D_EXACT_NAMLEN
                assert (_D_EXACT_NAMLEN(ent) == 1);
#endif
#ifdef _D_ALLOC_NAMLEN
                assert (_D_ALLOC_NAMLEN(ent) > 1);
#endif
                found += 1;

            } else if (strcmp (ent->d_name, "..") == 0) {
                /* Parent directory */
#ifdef _DIRENT_HAVE_D_TYPE
                assert (ent->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
                assert (ent->d_namlen == 2);
#endif
#ifdef _D_EXACT_NAMLEN
                assert (_D_EXACT_NAMLEN(ent) == 2);
#endif
#ifdef _D_ALLOC_NAMLEN
                assert (_D_ALLOC_NAMLEN(ent) > 2);
#endif
                found += 2;

            } else if (strcmp (ent->d_name, "file") == 0) {
                /* Regular file */
#ifdef _DIRENT_HAVE_D_TYPE
                assert (ent->d_type == DT_REG);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
                assert (ent->d_namlen == 4);
#endif
#ifdef _D_EXACT_NAMLEN
                assert (_D_EXACT_NAMLEN(ent) == 4);
#endif
#ifdef _D_ALLOC_NAMLEN
                assert (_D_ALLOC_NAMLEN(ent) > 4);
#endif
                found += 4;

            } else if (strcmp (ent->d_name, "dir") == 0) {
                /* Just a directory */
#ifdef _DIRENT_HAVE_D_TYPE
                assert (ent->d_type == DT_DIR);
#endif
#ifdef _DIRENT_HAVE_D_NAMLEN
                assert (ent->d_namlen == 3);
#endif
#ifdef _D_EXACT_NAMLEN
                assert (_D_EXACT_NAMLEN(ent) == 3);
#endif
#ifdef _D_ALLOC_NAMLEN
                assert (_D_ALLOC_NAMLEN(ent) > 3);
#endif
                found += 8;

            } else {
                /* Other file */
                cerr << "Unexpected file " << ent->d_name << endl;
                abort ();
            }

        }

        /* Make sure that all files were found */
        assert (found == 0xf);

        closedir (dir);
    }

    cout << "OK" << endl;
    return EXIT_SUCCESS;
}

