# Dou OS 2.1 English assets

`photo_assets.png` was created for this project using the built-in Imagegen tool. It contains a desert wallpaper in the left half and an original fictional portrait in the right half. It is an asset sheet, not an image of an actual product. The user's reference screenshot itself is not bundled.

Prompt:

> Create a single original photographic asset sheet for a retro handheld phone-interface project. Landscape canvas 1536 by 1024. NO TEXT, NO LETTERS, NO ICONS, NO PHONE, NO UI. Use two clean rectangular panels with a perfectly straight vertical boundary at exactly the midpoint of the canvas and no gap. LEFT HALF: an elegant iOS-style desert wallpaper, minimalist realistic sweeping pale golden sand dunes and soft hazy mountains under a muted pale blue sky, fine sand ripples in the foreground, tranquil premium photographic finish, horizon around upper 45 percent. RIGHT HALF: a completely original editorial portrait of a fictional young adult woman with short dark brown hair, holding an ivory pleated paper fan with a few magenta petals across the lower half of her face, expressive eyes looking toward camera, textured dusty lavender background, cinematic natural light. Portrait composition fills this right panel, no visible border. These are two raw image assets, NOT a screen mockup. Keep all important portrait content in the middle of the right panel. High visual quality, natural colors, no watermarks.

The build generator crops, resizes and quantizes this sheet for the DS's 15-bit output. App icons are original code-drawn vector shapes rasterized at 4× and reduced to 32×32 RGBA. Glass backgrounds, layout, text and controls are drawn by the native C renderer.

`fonts/Inter.ttf` is the unmodified Inter variable font obtained from the official Google Fonts repository:
https://github.com/google/fonts/tree/main/ofl/inter

License: `../licenses/Inter-OFL.txt`. The font atlas is generated at 7, 9, 11, 15, 22, 32 and 44 pixels.

The calendar icon uses the English abbreviation FRI. All runtime UI labels, messages, and dates are rendered in English by the C application.
