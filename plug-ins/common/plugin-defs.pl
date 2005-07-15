%plugins = (
    'AlienMap2' => { libdep => 'gtk', ui => 1 },
    'CEL' => { libdep => 'gtk', ui => 1 },
    'CML_explorer' => { libdep => 'gtk', ui => 1 },
    'aa' => { libdep => 'gtk', ui => 1, optional => 1 },
    'align_layers' => { libdep => 'gtk', ui => 1 },
    'animationplay' => { libdep => 'gtk', ui => 1 },
    'animoptimize' => { libdep => 'glib' },
    'apply_lens' => { libdep => 'gtk', ui => 1 },
    'autocrop' => { libdep => 'glib' },
    'autostretch_hsv' => { libdep => 'glib' },
    'blinds' => { libdep => 'gtk', ui => 1 },
    'blur' => { libdep => 'glib' },
    'borderaverage' => { libdep => 'gtk', ui => 1 },
    'bumpmap' => { libdep => 'gtk', ui => 1 },
    'c_astretch' => { libdep => 'glib' },
    'cartoon' => { libdep => 'gtk', ui => 1 },
    'ccanalyze' => { libdep => 'gtk', ui => 1 },
    'channel_mixer' => { libdep => 'gtk', ui => 1 },
    'checkerboard' => { libdep => 'gtk', ui => 1 },
    'color_enhance' => { libdep => 'gtk', ui => 1 },
    'colortoalpha' => { libdep => 'gtk', ui => 1 },
    'colorify' => { libdep => 'gtk', ui => 1 },
    'compose' => { libdep => 'gtk', ui => 1 },
    'compressor' => { libdep => 'glib' },
    'convmatrix' => { libdep => 'gtk', ui => 1 },
    'csource' => { libdep => 'gtk', ui => 1 },
    'cubism' => { libdep => 'gtk', ui => 1 },
    'curve_bend' => { libdep => 'gtk', ui => 1 },
    'decompose' => { libdep => 'gtk', ui => 1 },
    'deinterlace' => { libdep => 'gtk', ui => 1 },
    'depthmerge' => { libdep => 'gtk', ui => 1 },
    'despeckle' => { libdep => 'gtk', ui => 1 },
    'destripe' => { libdep => 'gtk', ui => 1 },
    'dicom' => { libdep => 'gtk', ui => 1 },
    'diffraction' => { libdep => 'gtk', ui => 1 },
    'displace' => { libdep => 'gtk', ui => 1 },
    'dog' => { libdep => 'gtk', ui => 1 },
    'edge' => { libdep => 'gtk', ui => 1 },
    'emboss' => { libdep => 'gtk', ui => 1 },
    'engrave' => { libdep => 'gtk', ui => 1 },
    'exchange' => { libdep => 'gtk', ui => 1 },
    'film' => { libdep => 'gtk', ui => 1 },
    'flarefx' => { libdep => 'gtk', ui => 1 },
    'fp' => { libdep => 'gtk', ui => 1 },
    'fractaltrace' => { libdep => 'gtk', ui => 1 },
    'gauss' => { libdep => 'gtk', ui => 1 },
    'gbr' => { libdep => 'gtk', ui => 1 },
    'gee' => { libdep => 'gtk', ui => 1 },
    'gee_zoom' => { libdep => 'gtk', ui => 1 },
    'gif' => { libdep => 'gtk', ui => 1 },
    'gifload' => { libdep => 'gtk' },
    'gih' => { libdep => 'gtk', ui => 1 },
    'glasstile' => { libdep => 'gtk', ui => 1 },
    'glob' => { libdep => 'glib' },
    'gqbist' => { libdep => 'gtk', ui => 1 },
    'gradmap' => { libdep => 'glib', ui => 1 },
    'grid' => { libdep => 'gtk', ui => 1 },
    'gtm' => { libdep => 'gtk', ui => 1 },
    'guillotine' => { libdep => 'glib' },
    'header' => { libdep => 'gtk', ui => 1 },
    'hot' => { libdep => 'gtk', ui => 1 },
    'illusion' => { libdep => 'gtk', ui => 1 },
    'iwarp' => { libdep => 'gtk', ui => 1 },
    'jigsaw' => { libdep => 'gtk', ui => 1 },
    'laplace' => { libdep => 'glib', ui => 1 },
    'lic' => { libdep => 'gtk', ui => 1 },
    'mail' => { libdep => 'gtk', ui => 1, optional => 1 },
    'mapcolor' => { libdep => 'gtk', ui => 1 },
    'max_rgb' => { libdep => 'gtk', ui => 1 },
    'mblur' => { libdep => 'gtk', ui => 1 },
    'mng' => { libdep => 'gtk', ui => 1, optional => 1 },
    'mosaic' => { libdep => 'gtk', ui => 1 },
    'neon' => { libdep => 'gtk', ui => 1 },
    'newsprint' => { libdep => 'gtk', ui => 1 },
    'nlfilt' => { libdep => 'gtk', ui => 1 },
    'noisify' => { libdep => 'gtk', ui => 1 },
    'normalize' => { libdep => 'glib' },
    'nova' => { libdep => 'gtk', ui => 1 },
    'oilify' => { libdep => 'gtk', ui => 1 },
    'papertile' => { libdep => 'gtk', ui => 1 },
    'pat' => { libdep => 'gtk', ui => 1 },
    'pcx' => { libdep => 'gtk', ui => 1 },
    'photocopy' => { libdep => 'gtk', ui => 1 },
    'pix' => { libdep => 'gtk', ui => 1 },
    'pixelize' => { libdep => 'gtk', ui => 1 },
    'plasma' => { libdep => 'gtk', ui => 1 },
    'png' => { libdep => 'gtk', ui => 1, optional => 1 },
    'pnm' => { libdep => 'gtk', ui => 1 },
    'polar' => { libdep => 'gtk', ui => 1 },
    'poppler' => { libdep => 'gtk', ui => 1, optional => 1 },
    'postscript' => { libdep => 'gtk', ui => 1 },
    'psd' => { libdep => 'glib' },
    'psd_save' => { libdep => 'gtk', ui => 1 },
    'psp' => { libdep => 'gtk', ui => 1, optional => 1, libopt => 'z' },
    'randomize' => { libdep => 'gtk', ui => 1 },
    'raw' => { libdep => 'gtk', ui => 1 },
    'retinex' => { libdep => 'gtk', ui => 1 },
    'ripple' => { libdep => 'gtk', ui => 1 },
    'rotate' => { libdep => 'glib' },
    'sample_colorize' => { libdep => 'gtk', ui => 1 },
    'scatter_hsv' => { libdep => 'gtk', ui => 1 },
    'screenshot' => { libdep => 'gtk', ui => 1, optional => 1 },
    'sel_gauss' => { libdep => 'gtk', ui => 1 },
    'semiflatten' => { libdep => 'glib' },
    'sharpen' => { libdep => 'gtk', ui => 1 },
    'shift' => { libdep => 'gtk', ui => 1 },
    'sinus' => { libdep => 'gtk', ui => 1 },
    'smooth_palette' => { libdep => 'gtk', ui => 1 },
    'snoise' => { libdep => 'gtk', ui => 1 },
    'sobel' => { libdep => 'gtk', ui => 1 },
    'softglow' => { libdep => 'gtk', ui => 1 },
    'sparkle' => { libdep => 'gtk', ui => 1 },
    'spheredesigner' => { libdep => 'gtk', ui => 1 },
    'spread' => { libdep => 'gtk', ui => 1 },
    'struc' => { libdep => 'gtk', ui => 1 },
    'sunras' => { libdep => 'gtk', ui => 1 },
    'svg' => { libdep => 'gtk', ui => 1, optional => 1 },
    'tga' => { libdep => 'gtk', ui => 1 },
    'threshold_alpha' => { libdep => 'gtk', ui => 1 },
    'tiff' => { libdep => 'gtk', ui => 1, optional => 1 },
    'tile' => { libdep => 'gtk', ui => 1 },
    'tileit' => { libdep => 'gtk', ui => 1 },
    'tiler' => { libdep => 'glib' },
    'uniteditor' => { libdep => 'gtk', ui => 1 },
    'unsharp' => { libdep => 'gtk', ui => 1 },
    'video' => { libdep => 'gtk', ui => 1 },
    'vinvert' => { libdep => 'glib' },
    'vpropagate' => { libdep => 'gtk', ui => 1 },
    'warp' => { libdep => 'gtk', ui => 1 },
    'waves' => { libdep => 'gtk', ui => 1 },
    'webbrowser' => { libdep => 'glib' },
    'whirlpinch' => { libdep => 'gtk', ui => 1 },
    'wind' => { libdep => 'gtk', ui => 1 },
    'winprint' => { libdep => 'glib', ui => 1, optional => 1 },
    'wmf' => { libdep => 'gtk', ui => 1, optional => 1 },
    'xbm' => { libdep => 'gtk', ui => 1 },
    'xpm' => { libdep => 'gtk', ui => 1, optional => 1 },
    'xwd' => { libdep => 'gtk', ui => 1 },
    'zealouscrop' => { libdep => 'glib' }
);
