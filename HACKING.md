Building
========

Dependencies

    * glib 2.12 or later
    * D-Bus 1.0 and Glib bindings
    * PAM
    * ConsoleKit 0.2.3
    * gnome-vfs (for file monitoring in greeter)

Submit changes in a fork and pull request them to the mate-1.26 branch.

Making Changes
==============


 * Patches should not introduce additional compilation warnings.

 * Patches must use the MDM Coding Style.


Coding Style
============

 * Follow the coding style already used.
 * Trailing whitespace is not allowed.
 * Use spaces (not tabs) for indentation.
 * Brace on same line as block statement.
 * Use braces on all blocks (even single line blocks).
 * Code must conform to C89/C90 (ANSI/ISO C) (ie. no C99).
 * Spaces between functions and arguments, or macros and arguments.
 * Spaces before and after most binary operators.
 * Spaces after most unary operators (including a comma).
 * Return value on line before function definition.
 * Brace on line after function definition.
 * Do not initialize local variables in their declarations.
 * Line up function arguments in declarations.
 * One variable declaration per line.
 * Line up variable names in declarations.
 * Limit the use of function prototypes by defining functions
   in the file before they are used.
 * Whenever possible conditional expressions should not have side
   effects (eg. don't set a value or call a function in an if
   statement).
 * '*' goes with variable not type, when declaring a pointer.
 * Function arguments follow the function call on the same line, and if
   lengthy are (where possible) wrapped to the column of the first brace.
 * Always compare pointers with NULL instead of using "!".
 * If you make a non-trivial and copyrightable change to a file
   be sure to add your name to the copyright information at the
   beginning of the file.
 * Use gtk-doc style comments on public functions whenever possible.
 * In most cases, don't abbreviate words in function and variable names.
 * Function names are lowercase, words separated by underscores.
 * Macros and enums are all uppercase, words seperated by
   underscores.
 * Types are all words capitalized, no separators between words.

 * Don't use gchar.  "char" is just fine.
 * Public functions should check input using g_return_if_fail.
 * Private functions should check input using g_assert.
 * Handle the default case in switch statements with
   a warning or g_assert_not_reached.
 * Prefer glib functions over native ones when available.
 * Use glib string handling functions when possible.

 * When in doubt copy the style of the rest of the file.
