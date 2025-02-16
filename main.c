/* 
 * File:   main.c
 * Author: Hypnotriod
 */

#include <stdio.h>
#include "c99_string.h"

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

static const string_t str_global = str_static("My global statically allocated string");
static const string_t COMMA = str_static(", ");
static const string_t DOT = str_static(".");

int main() {
    print_string(&str_global);

    string_t str_local = str_static("My local statically allocated string");
    print_string(&str_local);

    string_t* str_concatenated = str_concat(&str_global, &str_local);
    print_string(str_concatenated);

    string_t* str_sliced = str_slice(str_concatenated, -28, 12);
    print_string(str_sliced);

    string_t* str_dynamic = str_new("My dynamic string");
    print_string(str_dynamic);

    string_t what1 = str_static("My");
    string_t to1 = str_static("123");
    string_t* str_replaced1 = str_replace(str_dynamic, &what1, &to1);
    print_string(str_replaced1);

    string_t what2 = str_static("string");
    string_t to2 = str_static("789");
    string_t* str_replaced2 = str_replace(str_replaced1, &what2, &to2);
    print_string(str_replaced2);

    string_t what3 = str_static(" dynamic ");
    string_t to3 = str_static("456");
    string_t* str_replaced3 = str_replace(str_replaced2, &what3, &to3);
    print_string(str_replaced3);

    string_t str_to_replace_all = str_static("test... this is a test. (testtest) This test is simple. tes");
    string_t what = str_static("test");
    string_t to = str_static("example");
    string_t* str_replaced_all = str_replace_all(&str_to_replace_all, &what, &to);
    print_string(str_replaced_all);

    int str_index_of_result = str_index_of(str_dynamic, &what2);
    printf("index of 'string' in 'My dynamic string' is %i\r\n", str_index_of_result);

    bool str_contains_result = str_contains(str_dynamic, &what2);
    printf("'My dynamic string' contains 'string': %s\r\n", str_contains_result ? "true" : "false");

    string_t str_file_name = str_static("test.file.name.txt");
    str_index_of_result = str_last_index_of(&str_file_name, &DOT);
    string_t* str_file_extension = str_slice(&str_file_name, str_index_of_result, -1);
    print_string(str_file_extension);

    // string_t* str_formatted = str_new_format(100, "%s, %s, %s", str_unwrap(&str_global), str_unwrap(&str_local), str_unwrap(str_dynamic));
    string_t* str_formatted = str_new_format(100, "%s, %s, %s", str_global.c, str_local.c, str_dynamic->c);
    print_string(str_formatted);

    string_t* str_concatenated_n = str_concat_n(5, &str_global, &COMMA, &str_local, &COMMA, str_dynamic);
    print_string(str_concatenated_n);

    string_t* str_joined_n = str_join_n(&COMMA, 3, &str_global, &str_local, str_dynamic);
    print_string(str_joined_n);

    bool str_equals_result = str_equals(str_concatenated_n, str_joined_n);
    printf("strings are equal: %s\r\n", str_equals_result ? "true" : "false");

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
