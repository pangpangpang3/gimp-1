%plugins = (
    'CEL' => { libdep => 'gtk' },
    'CML_explorer' => { libdep => 'gtk' },
    'aa' => { libdep => 'gtk', ui => 1, optional => 1 },
    'align_layers' => { libdep => 'gtk' },
    'animationplay' => { libdep => 'gtk' },
    'animoptimize' => { libdep => 'glib' },
    'apply_lens' => { libdep => 'gtk' },
    'autocrop' => { libdep => 'glib' },
    'autostretch_hsv' => { libdep => 'glib' },
    'blinds' => { libdep => 'gtk' },
    'blur' => { libdep => 'gtk', libsupp => 'gpc' },
    'bumpmap' => { libdep => 'gtk', ui => 1 },
    'bz2' => { libdep => 'glib' },
    'c_astretch' => { libdep => 'glib' },
    'checkerboard' => { libdep => 'gtk' },
    'color_enhance' => { libdep => 'gtk', ui => 1 },
    'colorify' => { libdep => 'gtk' },
    'compose' => { libdep => 'gtk', ui => 1 },
    'convmatrix' => { libdep => 'gtk' },
    'csource' => { libdep => 'gtk', ui => 1 },
    'cubism' => { libdep => 'gtk' },
    'curve_bend' => { libdep => 'gtk' },
    'decompose' => { libdep => 'gtk' },
    'deinterlace' => { libdep => 'gtk', libsupp => 'megawidget' },
    'depthmerge' => { libdep => 'gtk', ui => 1 },
    'despeckle' => { libdep => 'gtk' },
    'destripe' => { libdep => 'gtk' },
    'diffraction' => { libdep => 'gtk' },
    'displace' => { libdep => 'gtk', ui => 1 },
    'edge' => { libdep => 'gtk' },
    'emboss' => { libdep => 'gtk', libsupp => 'megawidget' },
    'engrave' => { libdep => 'gtk' },
    'exchange' => { libdep => 'gtk' },
    'film' => { libdep => 'gtk' },
    'flarefx' => { libdep => 'gtk' },
    'fractaltrace' => { libdep => 'gtk' },
    'gauss_iir' => { libdep => 'gtk' },
    'gauss_rle' => { libdep => 'gtk' },
    'gbr' => { libdep => 'gtk', ui => 1 },
    'gpb' => { libdep => 'gtk', ui => 1 },
    'gee' => { libdep => 'gtk' },
    'gicon' => { libdep => 'gtk' },
    'gif' => { libdep => 'gtk', ui => 1 },
    'gifload' => { libdep => 'gtk' },
    'glasstile' => { libdep => 'gtk' },
    'gqbist' => { libdep => 'gtk' },
    'gradmap' => { libdep => 'glib' },
    'grid' => { libdep => 'gtk', ui => 1 },
    'gtm' => { libdep => 'gtk' },
    'guillotine' => { libdep => 'glib' },
    'gz' => { libdep => 'glib' },
    'header' => { libdep => 'glib' },
    'hot' => { libdep => 'gtk', libsupp => 'megawidget' },
    'hrz' => { libdep => 'gtk' },
    'illusion' => { libdep => 'gtk' },
    'iwarp' => { libdep => 'gtk' },
    'jigsaw' => { libdep => 'gtk' },
    'jpeg' => { libdep => 'gtk', ui => 1, optional => 1 },
    'laplace' => { libdep => 'glib' },
    'lic' => { libdep => 'gtk', ui => 1 },
    'mail' => { libdep => 'gtk' },
    'mapcolor' => { libdep => 'gtk' },
    'max_rgb' => { libdep => 'gtk' },
    'mblur' => { libdep => 'gtk' },
    'mpeg' => { libdep => 'gtk', optional => 1 },
    'newsprint' => { libdep => 'gtk' },
    'nlfilt' => { libdep => 'gtk', libsupp => 'megawidget' },
    'noisify' => { libdep => 'gtk' },
    'normalize' => { libdep => 'glib' },
    'nova' => { libdep => 'gtk' },
    'oilify' => { libdep => 'gtk' },
    'papertile' => { libdep => 'gtk' },
    'pat' => { libdep => 'gtk' },
    'pcx' => { libdep => 'glib' },
    'pix' => { libdep => 'glib' },
    'pixelize' => { libdep => 'gtk' },
    'plasma' => { libdep => 'gtk' },
    'plugindetails' => { libdep => 'gtk' },
    'png' => { libdep => 'gtk', ui => 1, optional => 1 },
    'pnm' => { libdep => 'gtk' },
    'polar' => { libdep => 'gtk' },
    'ps' => { libdep => 'gtk', ui => 1 },
    'psd' => { libdep => 'glib' },
    'psp' => { libdep => 'gtk', optional => 1, libopt => 'z' },
    'randomize' => { libdep => 'gtk', libsupp => 'gpc' },
    'ripple' => { libdep => 'gtk' },
    'rotate' => { libdep => 'gtk' },
    'rotators' => { libdep => 'glib' },
    'sample_colorize' => { libdep => 'gtk', ui => 1 },
    'scatter_hsv' => { libdep => 'gtk' },
    'screenshot' => { libdep => 'gtk' },
    'sel_gauss' => { libdep => 'gtk' },
    'semiflatten' => { libdep => 'glib' },
    'sharpen' => { libdep => 'gtk' },
    'shift' => { libdep => 'gtk' },
    'smooth_palette' => { libdep => 'gtk' },
    'snoise' => { libdep => 'gtk' },
    'sobel' => { libdep => 'gtk' },
    'sparkle' => { libdep => 'gtk' },
    'spheredesigner' => { libdep => 'gtk' },
    'spread' => { libdep => 'gtk' },
    'sunras' => { libdep => 'gtk' },
    'tga' => { libdep => 'gtk', ui => 1 },
    'threshold_alpha' => { libdep => 'gtk' },
    'tiff' => { libdep => 'gtk', ui => 1, optional => 1 },
    'tile' => { libdep => 'gtk' },
    'tileit' => { libdep => 'gtk' },
    'tiler' => { libdep => 'glib' },
    'url' => { libdep => 'glib' },
    'video' => { libdep => 'gtk' },
    'vinvert' => { libdep => 'glib' },
    'vpropagate' => { libdep => 'gtk' },
    'warp' => { libdep => 'gtk', ui => 1 },
    'waves' => { libdep => 'gtk', libsupp => 'megawidget' },
    'whirlpinch' => { libdep => 'gtk' },
    'wind' => { libdep => 'gtk' },
    'wmf' => { libdep => 'gtk' },
    'xbm' => { libdep => 'gtk' },
    'xpm' => { libdep => 'gtk', ui => 1, optional => 1 },
    'xwd' => { libdep => 'glib' },
    'zealouscrop' => { libdep => 'glib' }
);

@extra = qw(
    winclipboard.c
    winprint.c
);
