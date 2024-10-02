# EfficientPngLoading

## Format C++ code

```bash
apt install clang-format -y
clang-format -style=microsoft -dump-config > .clang-format
sed -i 's/AlwaysBreakTemplateDeclarations: MultiLine/AlwaysBreakTemplateDeclarations: Yes/g' .clang-format
sed -i 's/ColumnLimit:     120/ColumnLimit:     200/g' .clang-format

# To use
find . -regex '.*\.\(c\|cc\|cpp\|cxx\|cu\|h\|hh\|hpp\|hxx\|inl\|inc\|ipp\|m\|mm\)$' -exec clang-format -style=file -i {} \;
```


## Pre-commit

```
python3 -m pip install pre-commit
pre-commit install      # runs every time you commit in git
pre-commit run -a       # To use
pre-commit autoupdate   # To update this file
```

## Todo

- [x] IHDR chunk
- [x] PLTE chunk
- [x] IDAT chunk
- [x] IEND chunk
- [x] bKGD chunk
- [x] cHRM chunk
- [ ] cICP chunk
- [ ] dSIG chunk
- [ ] eXIf chunk
- [ ] gAMA chunk
- [ ] hIST chunk
- [ ] iCCP chunk
- [ ] iTXt chunk
- [x] pHYs chunk
- [ ] sBIT chunk
- [ ] sPLT chunk
- [ ] sRGB chunk
- [ ] sTER chunk
- [ ] tEXt chunk
- [ ] tIME chunk
- [ ] tRNS chunk
- [ ] zTXt chunk
