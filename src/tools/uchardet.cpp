/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Universal charset detector code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *          BYVoid <byvoid.kcp@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#include "../uchardet.h"
#include <cstdio>
#include <cxxopts.hpp>
#include <iostream>

#ifndef VERSION
#define VERSION "Unknown"
#endif
#define BUFFER_SIZE 65536

char buffer[BUFFER_SIZE];

void detect(FILE * fp)
{
    uchardet_t handle = uchardet_new();

    while (!feof(fp))
    {
        size_t len = fread(buffer, 1, BUFFER_SIZE, fp);
        int retval = uchardet_handle_data(handle, buffer, len);
        if (retval == HANDLE_DATA_RESULT_ERROR)
        {
            fprintf(stderr, "Handle data error.\n");
            exit(1);
        }
    }
    uchardet_data_end(handle);

    const char * charset = uchardet_get_charset(handle);
    float confidence = uchardet_get_confidence(handle);
    if (*charset)
    	printf("{ encoding=%s, confidence=%f }\n", charset, confidence);
    else
		printf("unknown\n");
	
    uchardet_delete(handle);
}

void show_version()
{
    printf("\n");
    printf("uchardet Command Line Tool\n");
    printf("Version %s\n", VERSION);
    printf("\n");
    printf("Authors: %s\n", "BYVoid, Jehan");
    printf("Bug Report: %s\n", "https://bugs.freedesktop.org/enter_bug.cgi?product=uchardet");
    printf("\n");
}

void show_usage()
{
    show_version();
    printf("Usage:\n");
    printf(" uchardet [Options] [File]...\n");
    printf("\n");
    printf("Options:\n");
    printf(" -v, --version         Print version and build information.\n");
    printf(" -h, --help            Print this help.\n");
    printf("\n");
}

int main(int argc, char ** argv)
{
    cxxopts::Options options("uchardet", "uchardet Command Line Tool");
    options.add_options()
        ("v,version", "show version")
        ("h,help", "print help")
        ("files", "input files", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"files"});
    auto result = options.parse(argc, argv);

    if (result.count("version"))
    {
        show_version();
        return 0;
    }
    if (result.count("help"))
    {
        show_usage();
        return 0;
    }

    int error_seen = 0;
    if (!result.count("files"))
    {
        // No file arg, use stdin by default
        size_t len = fread(buffer, 1, BUFFER_SIZE, stdin);
        int code = uchardet_detect_encoding(buffer, len);
        printf("%s\n", uchardet_charset_to_string(code));
    }
    else
    {
        const auto& files = result["files"].as<std::vector<std::string>>();
        for (const auto& filename : files)
        {
            FILE* f = fopen(filename.c_str(), "r");
            if (f == nullptr)
            {
                perror(filename.c_str());
                error_seen = 1;
                continue;
            }
            if (files.size() > 1)
            {
                printf("%s: ", filename.c_str());
            }
            detect(f);
            fclose(f);
        }
    }

    return error_seen;
}
