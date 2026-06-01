# Confirmed Constraints

- No extension upgrade path supported (`ALTER EXTENSION` returns syntax error).
- Aggregate functions (SUM/AVG) work via implicit casting if the output is numeric, but native aggregate support in VEF is limited to standard types.
