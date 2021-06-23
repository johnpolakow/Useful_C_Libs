
#ifndef _STRINGH_
#define _STRINGH_

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )    // macro to get number of items in an object array
#endif

class StringH
{
public:
  StringH();

  static char* Get_Substring(char* full_string, int start_index, int end_index);
  static int Parse_Int(char* integer_string);
  static double Parse_Double(char* double_string);
  static char* Parse_IP(char* line);
  static char* Parse_MAC(char* line);
  static int parse_USBTMC_Number(char* path);
  static void Trim_Leading_WhiteSpace(char* trim_string);
  static void Trim_Trailing_WhiteSpace(char * trim_string);
  static void Trim_WhiteSpace(char * trim_string);
  static void Erase_String(char* target_string);
  static void Erase_Num_Chars(char* string, int length);
  static char* Extract_Value_in_String(char* line);
  static int Index_First_Comma( char* string );
  static int Index_nTH_Delimiter( char* string, int n, char delimiter );
  static void To_Upper(char* lowercase_string);

  static int Index_Second_Whitespace( char* socket_string );
  static int Index_First_Whitespace( char* socket_string );
  static int Index_First_Sample(char* parse_string );
  static int Index_Second_Sample(char* parse_string );
  static int Index_First_Period( char* string );
  static bool String_Contains(char* haystack, const char* needle);
  static double Parse_Scientific_Notation( char* notation_string);
  static bool String_Contains_Ignore_Case(char* haystack, const char* needle);
  static int Index_of_E( char* string );
  static void Free_String_Array_Memory(char** string_array);
  static void Write_Chars(char* write_buffer, int* column_index, const char* print_buffer, int MAX_CHARS);
  static void Advance_to_Index(char* write_buffer, int* index_ptr, int target_index);
  static bool Contains_Only_Digits(char* digit_string);
  static bool String_Is_TimeStamp(char* time_string);
};

#endif
