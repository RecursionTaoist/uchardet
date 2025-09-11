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
#include "uchardet.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "nscore.h"
#include "nsUniversalDetector.h"

class HandleUniversalDetector : public nsUniversalDetector
{
protected:
    char *m_charset;
    float m_confidence;
public:
    HandleUniversalDetector()
    : nsUniversalDetector(NS_FILTER_ALL)
    , m_charset(0)
    {
        m_confidence = 0.0;
    }

    virtual ~HandleUniversalDetector()
    {
        if (m_charset) {
            free(m_charset);
            m_confidence = 0.0;
        }
    }

    virtual void Report(const char* charset, float confidence)
    {
        if (m_charset)
            free(m_charset);
        m_charset = strdup(charset);
        m_confidence = confidence;
    }

    virtual void Reset()
    {
        nsUniversalDetector::Reset();
        if (m_charset)
            free(m_charset);
        m_charset = strdup("");
        m_confidence = 0.0;
    }

    const char* GetCharset() const
    {
        return m_charset? m_charset : "";
    }

    float GetConfidence() {
        return m_confidence;
    }

    PRBool HasDone() {
        return mDone;
    }
};

uchardet_t uchardet_new(void)
{
    return reinterpret_cast<uchardet_t> (new HandleUniversalDetector());
}

void uchardet_delete(uchardet_t ud)
{
    delete reinterpret_cast<HandleUniversalDetector*>(ud);
}

int uchardet_handle_data(uchardet_t ud, const char * data, size_t len)
{
    nsresult ret = reinterpret_cast<HandleUniversalDetector*>(ud)->HandleData(data, (PRUint32)len);
    if (ret == NS_ERROR_OUT_OF_MEMORY) {
        return HANDLE_DATA_RESULT_ERROR;
    }

    if (reinterpret_cast<HandleUniversalDetector*>(ud)->HasDone()) {
        return HANDLE_DATA_RESULT_DETECTED;
    }

    return HANDLE_DATA_RESULT_NEED_MORE_DATA;
}

void uchardet_data_end(uchardet_t ud)
{
    reinterpret_cast<HandleUniversalDetector*>(ud)->DataEnd();
}

void uchardet_reset(uchardet_t ud)
{
    reinterpret_cast<HandleUniversalDetector*>(ud)->Reset();
}

const char* uchardet_get_charset(uchardet_t ud)
{
    return reinterpret_cast<HandleUniversalDetector*>(ud)->GetCharset();
}

float uchardet_get_confidence(uchardet_t ud)
{
    return reinterpret_cast<HandleUniversalDetector*>(ud)->GetConfidence();
}

/**
 * Converts uchardet/iconv-compatible encoding name string to charset code.
 * Unrecognized names will return `CHARSET_UNKNOWN`.
 *
 * @param charset encoding name string (case-sensitive).
 * @return corresponding charset code macro, `CHARSET_UNKNOWN` otherwise.
 */
int uchardet_charset_from_string(const char *charset)
{
    if (!charset) return CHARSET_UNKNOWN;
    // Compare most common first
    if (strcmp(charset, "UTF-8") == 0) return CHARSET_UTF8;
    if (strcmp(charset, "UTF-16BE") == 0) return CHARSET_UTF16BE;
    if (strcmp(charset, "UTF-16LE") == 0) return CHARSET_UTF16LE;
    if (strcmp(charset, "UTF-32BE") == 0) return CHARSET_UTF32BE;
    if (strcmp(charset, "UTF-32LE") == 0) return CHARSET_UTF32LE;
    if (strcmp(charset, "X-ISO-10646-UCS-4-34121") == 0) {
        return CHARSET_X_ISO10646_UCS4_34121;
    }
    if (strcmp(charset, "X-ISO-10646-UCS-4-21431") == 0) {
        return CHARSET_X_ISO10646_UCS4_21431;
    }
    if (strcmp(charset, "ISO-2022-CN") == 0) return CHARSET_ISO2022_CN;
    if (strcmp(charset, "BIG5") == 0) return CHARSET_BIG5;
    if (strcmp(charset, "EUC-TW") == 0) return CHARSET_EUC_TW;
    if (strcmp(charset, "GB18030") == 0) return CHARSET_GB18030;
    if (strcmp(charset, "HZ-GB-2312") == 0) return CHARSET_HZ_GB2312;
    if (strcmp(charset, "ISO-8859-1") == 0) return CHARSET_ISO8859_1;
    if (strcmp(charset, "ISO-8859-2") == 0) return CHARSET_ISO8859_2;
    if (strcmp(charset, "ISO-8859-3") == 0) return CHARSET_ISO8859_3;
    if (strcmp(charset, "ISO-8859-4") == 0) return CHARSET_ISO8859_4;
    if (strcmp(charset, "ISO-8859-5") == 0) return CHARSET_ISO8859_5;
    if (strcmp(charset, "ISO-8859-6") == 0) return CHARSET_ISO8859_6;
    if (strcmp(charset, "ISO-8859-7") == 0) return CHARSET_ISO8859_7;
    if (strcmp(charset, "ISO-8859-8") == 0) return CHARSET_ISO8859_8;
    if (strcmp(charset, "ISO-8859-9") == 0) return CHARSET_ISO8859_9;
    if (strcmp(charset, "ISO-8859-10") == 0) return CHARSET_ISO8859_10;
    if (strcmp(charset, "ISO-8859-13") == 0) return CHARSET_ISO8859_13;
    if (strcmp(charset, "ISO-8859-15") == 0) return CHARSET_ISO8859_15;
    if (strcmp(charset, "ISO-8859-16") == 0) return CHARSET_ISO8859_16;
    if (strcmp(charset, "WINDOWS-1250") == 0 ||
        strcmp(charset, "Windows-1250") == 0) {
        return CHARSET_WINDOWS_1250;
    }
    if (strcmp(charset, "WINDOWS-1251") == 0 ||
        strcmp(charset, "Windows-1251") == 0) {
        return CHARSET_WINDOWS_1251;
    }
    if (strcmp(charset, "WINDOWS-1252") == 0 ||
        strcmp(charset, "Windows-1252") == 0) {
        return CHARSET_WINDOWS_1252;
    }
    if (strcmp(charset, "WINDOWS-1253") == 0) return CHARSET_WINDOWS_1253;
    if (strcmp(charset, "WINDOWS-1255") == 0) return CHARSET_WINDOWS_1255;
    if (strcmp(charset, "WINDOWS-1256") == 0) return CHARSET_WINDOWS_1256;
    if (strcmp(charset, "WINDOWS-1257") == 0) return CHARSET_WINDOWS_1257;
    if (strcmp(charset, "WINDOWS-1258") == 0) return CHARSET_WINDOWS_1258;
    if (strcmp(charset, "KOI8-R") == 0) return CHARSET_KOI8R;
    if (strcmp(charset, "MAC-CYRILLIC") == 0) return CHARSET_MAC_CYRILLIC;
    if (strcmp(charset, "IBM852") == 0) return CHARSET_IBM852;
    if (strcmp(charset, "IBM855") == 0) return CHARSET_IBM855;
    if (strcmp(charset, "IBM866") == 0) return CHARSET_IBM866;
    if (strcmp(charset, "EUC-JP") == 0) return CHARSET_EUC_JP;
    if (strcmp(charset, "SHIFT_JIS") == 0) return CHARSET_SHIFT_JIS;
    if (strcmp(charset, "ISO-2022-JP") == 0) return CHARSET_ISO2022_JP;
    if (strcmp(charset, "ISO-2022-KR") == 0) return CHARSET_ISO2022_KR;
    if (strcmp(charset, "EUC-KR") == 0 || strcmp(charset, "UHC") == 0) {
        return CHARSET_EUC_KR;
    }
    if (strcmp(charset, "VISCII") == 0) return CHARSET_VISCII;
    if (strcmp(charset, "TIS-620") == 0) return CHARSET_TIS620;
    if (strcmp(charset, "ISO-8859-11") == 0) return CHARSET_ISO8859_11;
    if (strcmp(charset, "MacCentralEurope") == 0) {
        return CHARSET_MAC_CENTRALEUROPE;
    }
    if (strcmp(charset, "ASCII") == 0) return CHARSET_ASCII;

    return CHARSET_UNKNOWN;
}

/**
 * Converts charset code to uchardet/iconv-compatible encoding name string.
 * Unknown codes will return "UNKNOWN".
 *
 * @param code charset macro value.
 * @return encoding name string.
 */
const char *uchardet_charset_to_string(int code)
{
    switch (code) {
        case CHARSET_UTF8: return "UTF-8";
        case CHARSET_UTF16BE: return "UTF-16BE";
        case CHARSET_UTF16LE: return "UTF-16LE";
        case CHARSET_UTF32BE: return "UTF-32BE";
        case CHARSET_UTF32LE: return "UTF-32LE";
        case CHARSET_X_ISO10646_UCS4_34121: return "X-ISO-10646-UCS-4-34121";
        case CHARSET_X_ISO10646_UCS4_21431: return "X-ISO-10646-UCS-4-21431";
        case CHARSET_ISO2022_CN: return "ISO-2022-CN";
        case CHARSET_BIG5: return "BIG5";
        case CHARSET_EUC_TW: return "EUC-TW";
        case CHARSET_GB18030: return "GB18030";
        case CHARSET_HZ_GB2312: return "HZ-GB-2312";
        case CHARSET_ISO8859_1: return "ISO-8859-1";
        case CHARSET_ISO8859_2: return "ISO-8859-2";
        case CHARSET_ISO8859_3: return "ISO-8859-3";
        case CHARSET_ISO8859_4: return "ISO-8859-4";
        case CHARSET_ISO8859_5: return "ISO-8859-5";
        case CHARSET_ISO8859_6: return "ISO-8859-6";
        case CHARSET_ISO8859_7: return "ISO-8859-7";
        case CHARSET_ISO8859_8: return "ISO-8859-8";
        case CHARSET_ISO8859_9: return "ISO-8859-9";
        case CHARSET_ISO8859_10: return "ISO-8859-10";
        case CHARSET_ISO8859_13: return "ISO-8859-13";
        case CHARSET_ISO8859_15: return "ISO-8859-15";
        case CHARSET_ISO8859_16: return "ISO-8859-16";
        case CHARSET_WINDOWS_1250: return "WINDOWS-1250";
        case CHARSET_WINDOWS_1251: return "WINDOWS-1251";
        case CHARSET_WINDOWS_1252: return "WINDOWS-1252";
        case CHARSET_WINDOWS_1253: return "WINDOWS-1253";
        case CHARSET_WINDOWS_1255: return "WINDOWS-1255";
        case CHARSET_WINDOWS_1256: return "WINDOWS-1256";
        case CHARSET_WINDOWS_1257: return "WINDOWS-1257";
        case CHARSET_WINDOWS_1258: return "WINDOWS-1258";
        case CHARSET_KOI8R: return "KOI8-R";
        case CHARSET_MAC_CYRILLIC: return "MAC-CYRILLIC";
        case CHARSET_IBM852: return "IBM852";
        case CHARSET_IBM855: return "IBM855";
        case CHARSET_IBM866: return "IBM866";
        case CHARSET_EUC_JP: return "EUC-JP";
        case CHARSET_SHIFT_JIS: return "SHIFT_JIS";
        case CHARSET_ISO2022_JP: return "ISO-2022-JP";
        case CHARSET_ISO2022_KR: return "ISO-2022-KR";
        case CHARSET_EUC_KR: return "EUC-KR";
        case CHARSET_UHC: return "UHC";
        case CHARSET_VISCII: return "VISCII";
        case CHARSET_TIS620: return "TIS-620";
        case CHARSET_ISO8859_11: return "ISO-8859-11";
        case CHARSET_MAC_CENTRALEUROPE: return "MacCentralEurope";
        case CHARSET_ASCII: return "ASCII";
        default: return "UNKNOWN";
    }
}

/**
 * Detects the character encoding of the given data and returns its charset
 * macro code.
 *
 * @param data [in] pointer to the input data buffer.
 * @param len  [in] length of the input data in bytes.
 * @return charset macro code (e.g. `CHARSET_UTF8`) on success,
 *         `CHARSET_UNKNOWN` otherwise.
 * @note The returned code can be converted to a string via
 *       `uchardet_charset_to_string()`.
 */
int uchardet_detect_encoding(const char * data, size_t len)
{
    uchardet_t ud = uchardet_new();
    if (!ud) return CHARSET_UNKNOWN;

    int res = uchardet_handle_data(ud, data, len);
    if (res == HANDLE_DATA_RESULT_ERROR) {
        uchardet_delete(ud);
        return CHARSET_UNKNOWN;
    }

    uchardet_data_end(ud);
    const char* charset = uchardet_get_charset(ud);

    if (charset && charset[0] != '\0') {
        int code = uchardet_charset_from_string(charset);
        uchardet_delete(ud);
        return code;
    }

    uchardet_delete(ud);
    return CHARSET_UNKNOWN;
}
