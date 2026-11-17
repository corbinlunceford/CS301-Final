# CS 301 — Huffman ZIP301 Compression

Two C++17 command-line programs implementing the course's ZIP301 format.

## Build

```bash
g++ -std=c++17 -O2 -Wall -Wextra compress.cpp -o compress
g++ -std=c++17 -O2 -Wall -Wextra decompress.cpp -o decompress
```

## Run

```bash
./compress sample.txt
./decompress sample.zip301
cmp sample.txt sample2.txt
```

On Windows, compile to `compress.exe` and `decompress.exe`, and use `fc /b sample.txt sample2.txt` to compare.

## Format

Header: `prefix-code character-name` lines, followed by `*****`, then a decimal count of meaningful encoded bits, then a newline. Body: packed bits, most significant bit first, with trailing zero padding.

Reserved names: `space`, `newline`, `return`, `tab`.

## Implementation

Compression counts each input byte, builds a minimum-frequency Huffman tree with a priority queue, generates prefix codes, writes the plaintext header and packed binary body. Decompression rebuilds a decoding trie from the header and reads exactly the specified number of bits.

## Limitations / assumptions

The specified ZIP301 header is designed for ordinary plain-text input. It does not define escaping for arbitrary non-whitespace control characters or disambiguation for certain literal character labels. This implementation follows the specified four whitespace aliases and is intended for text files, not arbitrary binary input. The code is deterministic for ties by minimum character value. Empty input produces an empty code table and zero bits. A single unique character gets code `0`.

## Team write-up starter

**Algorithm:** Huffman coding repeatedly merges the two least frequent nodes into a binary tree. Each left branch represents 0 and each right branch represents 1. Frequent characters tend to receive shorter codes. The header makes the file self-decoding; the bit count distinguishes meaningful bits from trailing zero padding.

**Complexity:** For input length n and k distinct characters, counting is O(n), tree construction O(k log k), encoding O(number of encoded bits), and decoding O(number of encoded bits). Space is O(n+k) in this implementation because the compressor stores the input in memory.

**External assistance disclosure:** Initial implementation and documentation drafted with OpenAI ChatGPT; team should review, test, and adapt the code, and cite additional sources consulted.
