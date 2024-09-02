# Faser
The original idea was to have a lot of allpass filters in series just like phase dispersion plugins but I realized during development I could easily implement all the main filter types with it. Interface could be improved but it'll do for now.
If the plugin stops outputting audio after you stack a lot of low pass or high pass filters, simply decrease the slope until it works again and avoid using Q values above 0.7 in this situation.

![How the plugin currently looks](https://github.com/HyperLan-git/faser/blob/main/screenshot.PNG?raw=true)

## Dependencies
On windows, you just need Visual Studio 2022 and Juce.
In addition to juce's main dependencies on linux, you'll need libasan to compile in debug mode.

## How to compile
On windows, simply open the project from projucer and build the solution you need.

On linux, use the makefile :
```sh
make
```

To compile in Release mode (with optimisations and no memory sanitizer), use `make CONFIG=Release`.
You can clean binaries with `make clean`.
