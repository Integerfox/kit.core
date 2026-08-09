# Kit::Framing
@brief namespace description for Kit::Framing
@namespace Kit::Framing @brief

The Framing namespace provides interfaces for encoding and decoding streams
of data. A text frame is a sequence of bytes that are bounded by a unique
start-of-frame (SOF) and end-of-frame (EOF) byte.  There is also an 'escape`
(ESC) byte that is used when to transform a SOF, EOF, ESC bytes into a two
byte sequence so that SOF/EOF/ESC bytes are unique within a frame.

Below are some examples:

```
    Framing characters:
    Given:
        SOF:= '.'
        EOF:= ';'
        ESC:= '~'


        Raw Stream      Decoded Sequence(s)
        -----------     --------------------
        ".abcde;"       "abcde"
        ".a;.b;.;"      "a", "b", ""
        ".a;b.c;"       "a", "c"
        ".a~;bcd;"      "a;bcd"
        ".~~;"          "~"
        ".a~.bcd;"      "a.bcd"
```

Notes:

- The quotes (") in the above example are NOT part of the frame and/or character
  sequences - the quotes are only used to illustrate sets of characters.

- If an unescaped start-of-frame byte is encounter within a frame, the current
  in-progress frame is discarded, and a new frame is started.

- The SOF, EOF, and ESC bytes **must** all be unique values.
