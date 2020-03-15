# Pheromone

* [Overview](#overview)
* [Keymap](#keymap)
* [How to use](#how-to-use)
    * [Using rotary encoder with lightroom](#using-rotary-encoder-with-lightroom)
    * [Using slider with FL Studio](#using-slider-with-FL-Studio)
* [Build setup](#build-setup)
    * [Hardware](#hardware)
        * [PCB](#pcb)
        * [BOM](#bom)
    * [Software](#software)
        * [Bootloader](#bootloader)
        * [Flashing firmware](#flashing-firmware)
        * [Customizing oled screen](#customizing-oled-screen)
        * [Enable midi slider](#enable-midi-slider)
        * [Customizing email address marco](#customizing-email-address-marco)
* [Assembly](#assembly)
    * [Plate](#plate)
    * [Case](#case)
    * [Slider cap](#slider-cap)
* [Bugs and improvements](#bugs-and-improvements)

# Overview

Pheromone is a 63-key keyboard supports slide potentiometer, rotary encoder and oled screen. The slide potentiometer outputs a midi signal which can be recognized by any software reads midi input, however the stability is still being tested. The rotary encoder also outputs midi signal when the keyboard is switched to 'Lightroom' dedicated layer, once you pressed an alpha key the knob will output a midi cc signal with different parameter, which allows you use a knob to adjust up to 26 sliders in lightroom, furthermore, most importantly, the value will not jump or jitter, oppositely it moves relatively and smoothly. The oled screen will show you the last alpha key you pressed, the caps lock state and the layer you're currently staying. The firmware is fully QMK, see [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) then the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.

![PHOTO](https://i.imgur.com/w7OICen.jpg)

 Keyboard Maintainer: [luantty2](https://github.com/luantty2)

Make example for this keyboard (after setting up your build environment):

```
 make pheromone:default
```

[Back to top](#pheromone)

# Keymap

## Dimension

![Photo](https://i.imgur.com/tlR7r5q.png)

## Base layer

![Photo](https://i.imgur.com/cxCZICB.png)

## Function layer

![Photo](https://i.imgur.com/68KGHlW.png)
*The encoder in func layer has the abilitity to contol screen brightness, however it's only compatible with macos currently.*

## Lightroom layer

![Photo](https://i.imgur.com/Kzzc2MX.png)

[Back to top](#pheromone)

# How to use

## Using rotary encoder with lightroom

To enable midi input for lightroom, you first need to make sure [MIDI2LR](https://rsjaffe.github.io/MIDI2LR/) plugin installed on your computer, plug in keyboard and restart lightroom, you should see a configure page, in which you could combine the output of rotary encoder with lightroom command.

Before you start you should switch the keyboard layer to 'LTRM' by pressing "Fn" key twice, aware that the current layer you're staying is displayed on oled screen. 

The rotary encoder outputs values on CC1 by default, now if you rotate the encoder, the MIDI2LR page should appear a new row written MIDI command of 'CC1', if it doesn't, click 'Rescan MIDI devices' button below.

Now that our midi command isn't assigned to a lightroom command, to do that we simply click the row written in 'Unassigned', then it should appear a huge amount of command we could assign with, provided you have chosen 'Basic Tone -> Brightness', what it does is you're able to control 'brightness' by rotating encoder.

__It's important to right click on your command row, and select 'Binary offset' in the pop up window, and click 'apply to all'.__

I mentioned that the rotary encoder can handle up to 26 different sliders in lightroom, to do that, click any alpha key then rotate encoder, you should see a new row appear, assign it to any command you want, for example: press 'E' and assign command to 'Basic Tone -> Exposure'.

![PHOTO](https://i.imgur.com/EVTehiw.png)

## Using slider with FL Studio

Firstly, let's configure our keyboard as a midi input device in FL studio, go to options -> MIDI settings, click 'enable' button if 'pheromone' is shown here.

If you want to use slider to control the master volume, right click on the master volume icon, and select 'link to controller', move the slider on your keyboard then they should be automatically bound together.

[Back to top](#pheromone)

# Build setup

## Hardware

### PCB

The gerber files is ready to be manufactured, send it to any manufacturer you trust to be good. 

![PHOTO](https://i.imgur.com/Z5nTv9h.jpg)

### BOM

|Parts|Footprint|Quantity|
|:---|:---|:---|
|22pf capactior |0805|2|
|1uf capactior|0805|1|
|100nf capactior |0805|4|
|4.7uf capactior|0805|1|
|1N4148W diode|SOD-123|63|
|10K resistor |0805|2|
|22ohm resistor|0805|2|
|16Mhz Crystal |3225|1|
|reset switch |6mm*2.5mm|1|
|rotary encoder|EC11|1|
|Kailh hot swap socket||63|
|OLED screen|0.91' SSD1306|1|
|slide potentiometer  |See info below|1|
|ATmega32U4-AU |TQFP-44|1|
|USB micro |USB_Micro-B_Molex|1|
|key switch ||63|


The slide potentiometer I'm using here is bought from [taobao](https://detail.tmall.com/item.htm?id=605360822904&spm=a1z09.2.0.0.62ba2e8dWNJi1c&_u=110cp31q45a9), in theory any other B10K slide potentiometer should also work fine, just remember to modify the footprint in KiCad if your slide potentiometer come with a different size.
<!-- ![Photo](https://i.imgur.com/kBHOk4f.jpg) -->
<img src="https://i.imgur.com/kBHOk4f.jpg" width="300">

The handle length of rotary encoder would better be 15mm, so that it's easier to find a suitable cap for it, otherwise it would be too high or to low.

<!-- ![Photo](https://i.imgur.com/AZAtBvj.jpg) -->
<img src="https://i.imgur.com/AZAtBvj.jpg" width="300">

*The handle length refer to as the 'L' length*

![Photo](https://i.imgur.com/lK41vY3.jpg)
![Photo](https://i.imgur.com/Gm71eud.jpg)


[Back to top](#pheromone)

## Software

### Bootloader
The firmware uses qmk-dfu as its bootloader, once you've got MCU and other parts soldered, you need to set bootloader for your MCU, normally you will want a USBISP as ISP flasher, or a pro micro wired to ISP socket on your keyboard.


Here I'm using a teensy 2.0 flashed with [this](https://github.com/qmk/qmk_firmware/blob/master/util/teensy_2.0_ISP_B0.hex) firmware, to learn how to turn your teensy or pro micro into a ISP flasher and how to wire it to your keyboard, follow the [guide](https://beta.docs.qmk.fm/for-makers-and-modders/isp_flashing_guide).


Make sure you wired correctly, open up terminal and type:

```
ls /dev/tty.usb*
```

then it will show something like */dev/tty.usbmodem123456*

copy the result, run the command below:

```
avrdude -c avrisp -p m32u4 -P /dev/tty.usbmodem123456 -U flash:w:"pheromone_default_bootloader.hex":a -U lfuse:w:0x5E:m -U hfuse:w:0xD9:m -U efuse:w:0xC3:m -U lock:w:0x3F:m
```

### Flashing firmware

Re-plug in your keyboard, open [QMK-toolbox](https://qmk.fm/toolbox/), then press reset button on the back of PCB, the qmk toolbox will show you if it's detected our keyboard, if it does, simply find "pheromone_default.hex" and hit "flash" in qmk toolbox.

Once you've got qmk working, simply hold down esc when connecting keyboard to computer instead of pressing reset button to enter usb bootloader.

### Customizing oled screen

<!-- ![Photo](https://i.imgur.com/4qOjIy7.jpg) -->
<img src="https://i.imgur.com/4qOjIy7.jpg" width="200">

<!-- ![Photo](https://i.imgur.com/G7eADfx.jpg) -->
<img src="https://i.imgur.com/G7eADfx.jpg" width="200">

The oled screen always showing an image, in addition, it also shows you multiple useful information:
* the capslock state
* the last alpha key you pressed
* the layer you're currently staying
* the midi cc value of an alpha key if you're staying in 'LTRM' layer.

the font file is located in *keyboards/pheromone/keymaps/default/glcdfont.c*

Upload this file onto [helixfonteditor](https://helixfonteditor.netlify.com/), and you're able to modify the font and sprites. Once you've got your work done, overwrite the original file and recompile.

### Enable midi slider

The slider feature is not enabled by default, to enable it, edit *config.h* and uncomment the code below:
```
#define POT_ENABLE
```

You're able to detect the ouput value from using midi software, I'm using MIDI Monitor in this case.

![PHOTO](https://i.imgur.com/sUAwIdn.png)


**The feature is still being tested.**

### Customizing email address marco

When holding **right shift** and hit number '2', it will print an email address on screen, you can modify the address by editing line 559 in *keymap.c*:

```
SEND_STRING("your_email_address@xxx.com");
```
When holding left shift and number 2, it gives a '@' as usual.

[Back to top](#pheromone)

# Assembly

![Photo](https://i.imgur.com/y5gPgMj.png)

## Plate

![Photo](https://i.imgur.com/8Hc7hZv.jpg)
The plate file is ready to be manufactured, either carbon fiber or brass is fine.

There are 6 screw holes on plate and pcb, you can insert M2*10mm screws to enhance the tightness between switch and pcb, yet they are optional.

## Case
The case file is for 3D printing, either PLA or resin should be fine. 

After sanding and painting it actually gives you great surface. If you find some uneven gaps  on surface, woodfiller and primer will help you smooth them.

All screws for mounting holes are M2.5 size, but you can modify them up to the actual situation.

If you choose PLA then it's easy to heat insert threads, but if you choose to use resin, like me, it's a little tricky, you may want to glue your inset nuts to  every hole, and sanding nuts when necessary.

The buttom of acrylic is made of acrylic cut by laser.

## Slider cap
The cap of slider is also 3D printed.

[Back to top](#pheromone)

# Bugs and improvements

[Back to top](#pheromone)