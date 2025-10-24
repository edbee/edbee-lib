# Updating

## edbee 0.12.0 - modern types

To modernize edbee a lot code has been touched and core types have been changed.
This change is not fully backwards compatible.

- Size based data structures have changed to `size_t` and `ptrdiff`.
- Most methods which returned -1 to indicate a not found, when size_t is used, it will now return a `std::string::npos`.
- Most pointer 0's have been replaced with `nullptr`

