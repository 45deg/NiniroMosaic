# Niniro Mosaic

Realtime Photo-Mosaic Generator

Source code of http://titech-ssr.blog.jp/archives/1042202851.html

License: Public Domain, excluding licensed files.

## Requirements

- OpenCV 3.4.0
- GLFW 3.2.1

For macOS,
```
brew install opencv glfw3
cd source
make
```

I haven't tested all other operating systems.

## Execution

Prepare many JPEG images and put them in a directory. All the images must be the same size.

```
./NiniroMosaic -w 60 -h 40 -p 5 -s 30 -d image3
                  ^     ^     ^    ^     ^
                  +-----|-----|----|-----|------ Number of columns
                        +-----|----|-----|------ Number of rows
                              +----|-----|------ Resolution of image indexing (n^2)
                                   +-----|------ Size of tile images
                                         +------ Image folder
```

You can also specify a `-m` option with a movie file.
