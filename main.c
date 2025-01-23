/* 
 * File:   main.c
 * Author: Hypnotriod
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * The main immutable string structure
 */
typedef struct {
    const char* s;
    const int l;
} string_t;

/**
 * Flexible array members based string structure
 */
typedef struct {
    const char* s;
    int l;
    char data[];
} __string_fam_t;

// Static string_t initialization macro
#define STRS(__CHARS__) {.l = sizeof(__CHARS__) * sizeof(char) - 1, .s = __CHARS__}

// Unwrap string_t* into const char*
#define STRU(__STRING_t__) *(const char**)(__STRING_t__)

/**
 * Create new string_t* from the given characters and with the given length
 * @param chars - characters array
 * @param len - characters array length
 * @return string_t*
 */
string_t* str_new_len(const char* chars, int len) {
    __string_fam_t* str = malloc(sizeof (__string_fam_t) + len + 1);
    if (str == NULL) return NULL;
    memcpy(str->data, chars, len);
    str->data[len] = 0;
    str->s = str->data;
    str->l = len;
    return (string_t*) (str);
}

/**
 * Create new formatted string_t*
 * @param buff_size - the size of the variable-length array buffer
 * @param format - format characters
 * @param ... - format parameters
 * @return string_t*
 */
string_t* str_new_format(int buff_size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    char chars[buff_size];
    vsnprintf(chars, buff_size, format, ap);
    va_end(ap);
    int len = sizeof (char) * strlen(chars);
    return str_new_len(chars, len);
}

/**
 * Create new string_t* from the given characters
 * @param chars - characters array
 * @return string_t*
 */
string_t* str_new(const char* chars) {
    int len = sizeof (char) * strlen(chars);
    return str_new_len(chars, len);
}

/**
 * Concatenate n of string_t* into a new string_t*
 * @param n - number of string_t* to join
 * @param ... - string_t*
 * @return string_t*
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
    __string_fam_t* str = malloc(sizeof (__string_fam_t) + len + 1);
    if (str == NULL) return NULL;
    int offset = 0;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        string_t* s = va_arg(args, string_t*);
        memcpy(&str->data[offset], s->s, s->l);
        offset += s->l;
    }
    va_end(args);
    str->data[len] = 0;
    str->s = str->data;
    str->l = len;
    return (string_t*) (str);
}

/**
 * Concatenate two string_t* into a new string_t*
 * @param str1 - first string_t* to join
 * @param str2 - second string_t* to join
 * @return string_t*
 */
string_t* str_concat(const string_t* str1, const string_t* str2) {
    int len = str1->l + str2->l;
    __string_fam_t* str = malloc(sizeof (__string_fam_t) + len + 1);
    if (str == NULL) return NULL;
    memcpy(str->data, str1->s, str1->l);
    memcpy(&str->data[str1->l], str2->s, str2->l);
    str->data[len] = 0;
    str->s = str->data;
    str->l = len;
    return (string_t*) (str);
}

/**
 * Join n of string_t* into a new string_t* by the separator string_t*
 * @param n - number of string_t* to join
 * @param separator - string_t* separator
 * @param ... - string_t*
 * @return string_t*
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
    __string_fam_t* str = malloc(sizeof (__string_fam_t) + len + 1);
    if (str == NULL) return NULL;
    int offset = 0;
    va_start(args, n);
    for (int i = 0; i < n; i++) {
        string_t* s = va_arg(args, string_t*);
        memcpy(&str->data[offset], s->s, s->l);
        offset += s->l;
        if (i != n - 1) {
            memcpy(&str->data[offset], separator->s, separator->l);
            offset += separator->l;
        }
    }
    va_end(args);
    str->data[len] = 0;
    str->s = str->data;
    str->l = len;
    return (string_t*) (str);
}

/**
 * Slice string into a new string_t*
 * @param str - string to slice 
 * @param start - start character index
 * @param len - number of characters
 * @return string_t*
 */
string_t* str_slice(const string_t* str, int start, int len) {
    if (start < 0) start = str->l + start;
    if (start + len > str->l) len = str->l - start;
    if (len < 0 || start < 0 || start >= str->l) {
        return str_new_len("", 0);
    }
    return str_new_len(&str->s[start], len);
}

// *****************************
// ******* Usage example *******
// *****************************

void print_string(const string_t* str) {
    printf("\"%s\" has %i characters length\r\n", str->s, str->l);
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

    // string_t* str_formatted = str_new_format(100, "%s, %s, %s", STRU(&str_global), STRU(&str_local), STRU(str_dynamic));
    string_t* str_formatted = str_new_format(100, "%s, %s, %s", str_global.s, str_local.s, str_dynamic->s);
    print_string(str_formatted);

    string_t* str_concatenated_n = str_concat_n(5, &str_global, &COMMA, &str_local, &COMMA, str_dynamic);
    print_string(str_concatenated_n);

    string_t* str_joined_n = str_join_n(&COMMA, 3, &str_global, &str_local, str_dynamic);
    print_string(str_joined_n);

    string_t* str_input = prompt_string("> ", 100);
    print_string(str_input);

    free(str_concatenated);
    free(str_sliced);
    free(str_dynamic);
    free(str_formatted);
    free(str_concatenated_n);
    free(str_joined_n);
    free(str_input);

    return (EXIT_SUCCESS);
}
