/* 
 * File:   main.c
 * Author: Hypnotriod
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * The main immutable string structure
 */
typedef struct {
    // Characters array pointer
    const char* c;
    // Characters array length
    const int l;
} string_t;

/**
 * Flexible array members based string structure
 */
typedef struct {
    char* c;
    int l;
    char data[];
} __string_fam_t;

// __string_fam_t* memory allocation macro helper
#define __str_fam_malloc(__LEN__) malloc(sizeof (__string_fam_t) + ((__LEN__) + 1) * sizeof (char))

// Static string_t initialization macro
#define STRS(__CHARS__) {.l = sizeof((__CHARS__)) / sizeof(char) - 1, .c = (__CHARS__)}

// Unwrap string_t* into const char*
#define STRU(__STRING_t__) *(const char**)((__STRING_t__))

// Dynamic string_t* helper macro to free and set a null pointer
#define str_free(__STRING_t__) { free((__STRING_t__)); (__STRING_t__) = NULL; }

/**
 * Create new string_t* from the given characters and with the given length
 * @param chars - characters array
 * @param len - characters array length
 * @return new string_t*
 */
string_t* str_new_len(const char* chars, int len) {
    __string_fam_t* str = __str_fam_malloc(len);
    if (str == NULL) return NULL;
    memcpy(str->data, chars, len * sizeof (char));
    str->data[len] = '\0';
    str->c = str->data;
    str->l = len;
    return (string_t*) str;
}

/**
 * Create new formatted string_t* using variable-length array buffer
 * @param buff_size - the size of the variable-length array buffer
 * @param format - format characters
 * @param ... - format parameters
 * @return new string_t*
 */
string_t* str_new_format(int buff_size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    char chars[buff_size];
    int len = vsnprintf(chars, buff_size, format, ap);
    va_end(ap);
    if (len >= buff_size) len = buff_size;
    else if (len < 0) len = 0;
    return str_new_len(chars, len);
}

/**
 * Create new string_t* from the given characters
 * @param chars - characters array
 * @return new string_t*
 */
string_t* str_new(const char* chars) {
    int len = strlen(chars);
    return str_new_len(chars, len);
}

/**
 * Clone a string_t*
 * @param str - string_t* to clone
 * @return new string_t*
 */
string_t* str_clone(const string_t* str) {
    return str_new_len(str->c, str->l);
}

/**
 * Concatenate n of string_t* into a new string_t*
 * @param n - number of string_t* to join
 * @param ... - string_t*
 * @return new string_t*
 */
string_t* str_concat_n(int n, ...) {
    int len = 0;
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        string_t* s = va_arg(args, string_t*);
        len += s->l;
    }
    va_end(args);
    __string_fam_t* str = __str_fam_malloc(len);
    if (str == NULL) return NULL;
    int offset = 0;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        string_t* s = va_arg(args, string_t*);
        memcpy(&str->data[offset], s->c, s->l * sizeof (char));
        offset += s->l;
    }
    va_end(args);
    str->data[len] = '\0';
    str->c = str->data;
    str->l = len;
    return (string_t*) str;
}

/**
 * Concatenate two string_t* into a new string_t*
 * @param str1 - first string_t* to join
 * @param str2 - second string_t* to join
 * @return new string_t*
 */
string_t* str_concat(const string_t* str1, const string_t* str2) {
    int len = str1->l + str2->l;
    __string_fam_t* str = __str_fam_malloc(len);
    if (str == NULL) return NULL;
    memcpy(str->data, str1->c, str1->l * sizeof (char));
    memcpy(&str->data[str1->l], str2->c, str2->l * sizeof (char));
    str->data[len] = '\0';
    str->c = str->data;
    str->l = len;
    return (string_t*) str;
}

/**
 * Join n of string_t* into a new string_t* by the separator string_t*
 * @param n - number of string_t* to join
 * @param separator - string_t* separator
 * @param ... - string_t*
 * @return new string_t*
 */
string_t* str_join_n(const string_t* separator, int n, ...) {
    int len = 0;
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        string_t* s = va_arg(args, string_t*);
        len += s->l;
        if (i != n - 1) len += separator->l;
    }
    va_end(args);
    __string_fam_t* str = __str_fam_malloc(len);
    if (str == NULL) return NULL;
    int offset = 0;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        string_t* s = va_arg(args, string_t*);
        memcpy(&str->data[offset], s->c, s->l * sizeof (char));
        offset += s->l;
        if (i != n - 1) {
            memcpy(&str->data[offset], separator->c, separator->l * sizeof (char));
            offset += separator->l;
        }
    }
    va_end(args);
    str->data[len] = '\0';
    str->c = str->data;
    str->l = len;
    return (string_t*) str;
}

/**
 * Slice string into a new string_t*
 * @param str - string to slice 
 * @param start - start character index. Negative value represents before the end index
 * @param len - number of characters to slice. Any negative value means up to the remaining length
 * @return new string_t*
 */
string_t* str_slice(const string_t* str, int start, int len) {
    if (start < -str->l) start = 0;
    else if (start < 0) start = str->l + start;
    if (len < 0 || start + len > str->l) len = str->l - start;
    return str_new_len(&str->c[start], len);
}

/**
 * Trim ' ', '\t', '\n', '\v', '\f', '\r' characters from string_t*
 * @param str - string_t* to trim
 * @return new string_t*
 */
string_t* str_trim(const string_t* str) {
    int start = 0;
    int end = str->l;
    while (start != end && isspace(str->c[start]))
        start++;
    while (end != 0 && isspace(str->c[end - 1]))
        end--;
    return str_new_len(&str->c[start], end - start);
}

/**
 * Get index of first occurrence of substring string_t* in the given string_t*.
 * Returns -1 if nothing found or substring is empty string
 * @param str - string_t* input
 * @param substr - string_t* to search for
 * @return int
 */
int str_index_of(const string_t* str, const string_t* substr) {
    if (substr->l == 0) return -1;
    for (int i = 0; i < str->l; i++) {
        if (strncmp(&str->c[i], substr->c, substr->l) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Get index of last occurrence of substring string_t* in the given string_t*.
 * Returns -1 if nothing found or substring is empty string
 * @param str - string_t* input
 * @param substr - string_t* to search for
 * @return int
 */
int str_last_index_of(const string_t* str, const string_t* substr) {
    if (substr->l == 0) return -1;
    for (int i = str->l - 1; i >= 0; i--) {
        if (strncmp(&str->c[i], substr->c, substr->l) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Get number of occurrences of a substring string_t* in the given string_t*
 * @param str - string_t* input
 * @param substr - string_t* to search for
 * @return int
 */
int str_count(const string_t* str, const string_t* substr) {
    int i, count = 0;
    for (i = 0; i <= str->l - substr->l; i++) {
        if (strncmp(&str->c[i], substr->c, substr->l) == 0) {
            count++;
            i += substr->l - 1;
        }
    }
    return count;
}

/**
 * Replace first occurrence of string_t* 'what' with string_t* 'to' in string_t*
 * @param str - string_t* input
 * @param what - string_t* to search for
 * @param to - string_t* to replace with
 * @return new string_t*
 */
string_t* str_replace(const string_t* str, const string_t* what, const string_t* to) {
    if (what->l == 0) return str_new_len(str->c, str->l);
    int i, j;
    for (i = 0; i < str->l; i++) {
        if (strncmp(&str->c[i], what->c, what->l) == 0) {
            j = i + what->l;
            break;
        }
    }
    if (i < str->l) {
        int len = i + to->l + str->l - j;
        __string_fam_t* strnew = __str_fam_malloc(len);
        if (strnew == NULL) return NULL;
        memcpy(strnew->data, str->c, i * sizeof (char));
        memcpy(&strnew->data[i], to->c, to->l * sizeof (char));
        memcpy(&strnew->data[i + to->l], &str->c[j], (str->l - j) * sizeof (char));
        strnew->data[len] = '\0';
        strnew->c = strnew->data;
        strnew->l = len;
        return (string_t*) strnew;
    }
    return str_new_len(str->c, str->l);
}

/**
 * Replace all occurrences of string_t* 'what' with string_t* 'to' in string_t*
 * @param str - string_t* input
 * @param what - string_t* to search for
 * @param to - string_t* to replace with
 * @return new string_t*
 */
string_t* str_replace_all(const string_t* str, const string_t* what, const string_t* to) {
    if (what->l == 0) return str_new_len(str->c, str->l);
    int i, j = 0, k = 0, l = 0;
    int count = str_count(str, what);
    int len = str->l + count * (to->l - what->l);
    __string_fam_t* strnew = __str_fam_malloc(len);
    if (strnew == NULL) return NULL;
    for (i = 0; i < str->l;) {
        if (i <= str->l - what->l && strncmp(&str->c[i], what->c, what->l) == 0) {
            memcpy(&strnew->data[k], &str->c[l], (j - k) * sizeof (char));
            memcpy(&strnew->data[j], to->c, to->l * sizeof (char));
            j += to->l;
            i += what->l;
            k = j;
            l = i;
        } else {
            j++;
            i++;
        }
    }
    if (k != j) {
        memcpy(&strnew->data[k], &str->c[l], (j - k) * sizeof (char));
    }
    strnew->data[j] = '\0';
    strnew->c = strnew->data;
    strnew->l = len;
    return (string_t*) strnew;
}

// *****************************
// ******* Usage example *******
// *****************************

void print_string(const string_t* str) {
    printf("\"%s\" has %i characters length\r\n", str->c, str->l);
}

string_t* prompt_string(const char* prompt, int buff_size) {
    char buff[buff_size];
    memset(buff, 0, buff_size);
    printf("%s", prompt);
    scanf("%[^\n]s", buff);
    return str_new(buff);
}

static const string_t str_global = STRS("My global static string");
static const string_t COMMA = STRS(", ");
static const string_t DOT = STRS(".");

int main() {
    print_string(&str_global);

    string_t str_local = STRS("My local static string");
    print_string(&str_local);

    string_t* str_concatenated = str_concat(&str_global, &str_local);
    print_string(str_concatenated);

    string_t* str_sliced = str_slice(str_concatenated, -28, 8);
    print_string(str_sliced);

    string_t* str_dynamic = str_new("My dynamic string");
    print_string(str_dynamic);

    string_t what1 = STRS("My");
    string_t to1 = STRS("123");
    string_t* str_replaced1 = str_replace(str_dynamic, &what1, &to1);
    print_string(str_replaced1);

    string_t what2 = STRS("string");
    string_t to2 = STRS("789");
    string_t* str_replaced2 = str_replace(str_replaced1, &what2, &to2);
    print_string(str_replaced2);

    string_t what3 = STRS(" dynamic ");
    string_t to3 = STRS("456");
    string_t* str_replaced3 = str_replace(str_replaced2, &what3, &to3);
    print_string(str_replaced3);

    string_t str_to_replace_all = STRS("test... this is a test. (testtest) This test is simple. tes");
    string_t what = STRS("test");
    string_t to = STRS("example");
    string_t* str_replaced_all = str_replace_all(&str_to_replace_all, &what, &to);
    print_string(str_replaced_all);

    int index_of = str_index_of(str_dynamic, &what2);
    printf("index of 'string' in 'My dynamic string' is %i\r\n", index_of);

    string_t str_file_name = STRS("test.file.name.txt");
    index_of = str_last_index_of(&str_file_name, &DOT);
    string_t* str_file_extension = str_slice(&str_file_name, index_of, -1);
    print_string(str_file_extension);

    // string_t* str_formatted = str_new_format(100, "%s, %s, %s", STRU(&str_global), STRU(&str_local), STRU(str_dynamic));
    string_t* str_formatted = str_new_format(100, "%s, %s, %s", str_global.c, str_local.c, str_dynamic->c);
    print_string(str_formatted);

    string_t* str_concatenated_n = str_concat_n(5, &str_global, &COMMA, &str_local, &COMMA, str_dynamic);
    print_string(str_concatenated_n);

    string_t* str_joined_n = str_join_n(&COMMA, 3, &str_global, &str_local, str_dynamic);
    print_string(str_joined_n);

    string_t* str_input = prompt_string("> ", 100);
    print_string(str_input);

    string_t* str_cloned = str_clone(str_input);
    print_string(str_cloned);

    string_t* str_trimmed = str_trim(str_cloned);
    print_string(str_trimmed);

    str_free(str_concatenated);
    str_free(str_sliced);
    str_free(str_dynamic);
    str_free(str_replaced1);
    str_free(str_replaced2);
    str_free(str_replaced3);
    str_free(str_replaced_all);
    str_free(str_file_extension);
    str_free(str_formatted);
    str_free(str_concatenated_n);
    str_free(str_joined_n);
    str_free(str_input);
    str_free(str_cloned);
    str_free(str_trimmed);

    printf("str_input is pointing at %p after str_free call\r\n", str_input);
    // Test free after free
    str_free(str_input);

    return (EXIT_SUCCESS);
}
