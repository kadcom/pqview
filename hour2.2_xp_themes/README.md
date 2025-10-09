# Hour 2.2: Windows XP Themes and Visual Styles

## Objectives

- Enable Windows XP+ visual styles (themed controls)
- Use modern fonts (Segoe UI / Tahoma / Consolas)
- Enable ClearType font rendering
- Initialize Common Controls properly
- Embed manifest for theme support
- Modern look without changing layout

## Files

- `CMakeLists.txt` - Same as Hour 2.1
- `main.c` - Enhanced with XP themes and modern fonts

## What's New from Hour 2.1

### Visual Styles (Themes)

**Manifest Embedding:**
- Embedded manifest via pragma directive
- Enables Common Controls v6 (themed controls)
- Works on XP, Vista, 7, 8, 10, 11

**InitCommonControlsEx:**
- Properly initializes common controls
- Required for visual styles to work
- Must be called before creating any controls

### Modern Fonts

**Segoe UI** (Vista+):
- Modern, clean font from Windows Vista onward
- Falls back to Tahoma on XP
- Better than MS Shell Dlg for modern look

**Consolas** (Vista+):
- Modern monospace font
- ClearType-optimized
- Falls back to Courier New on XP

**ClearType Quality:**
- `CLEARTYPE_QUALITY` instead of `DEFAULT_QUALITY`
- Smoother, anti-aliased text rendering
- Much better readability

## Key Differences from Hour 2.1

| Aspect | Hour 2.1 | Hour 2.2 |
|--------|--------|----------|
| Visual Style | Classic (flat buttons) | Themed (XP+ style) |
| Font | MS Shell Dlg | Segoe UI / Tahoma |
| Monospace Font | Courier New | Consolas |
| Font Quality | DEFAULT_QUALITY | CLEARTYPE_QUALITY |
| Manifest | None | Embedded (Common Controls v6) |
| InitCommonControls | Not called | InitCommonControlsEx() |
| Button Style | Flat, gray | Gradient, blue hover |
| Group Box | Simple line | Themed border |

## Visual Appearance

**Windows XP:**
- Blue/green Luna theme
- Rounded buttons
- Gradient effects

**Windows Vista/7:**
- Aero glass effects
- Better fonts (Segoe UI, Consolas)
- Subtle gradients

**Windows 8/10/11:**
- Flat design
- Modern fonts
- Cleaner appearance
- Still uses themes engine

## Building

```bash
cd hour2.2_xp_themes
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DPostgreSQL_ROOT="C:/Program Files/PostgreSQL/17"
cmake --build . --config Release
Release\pqview.exe
```

## New Concepts

### Embedded Manifest

**What is a manifest?**
- XML file that tells Windows which DLL versions to use
- Can be external (.exe.manifest) or embedded in .exe
- Required for Common Controls v6 (themed controls)

**Embedding via pragma:**
```c
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' \
processorArchitecture='*' \
publicKeyToken='6595b64144ccf1df' \
language='*'\"")
```

This tells the linker to embed the manifest directly in the .exe.

**Alternative: External manifest file**
```xml
<!-- pqview.exe.manifest -->
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
  <dependency>
    <dependentAssembly>
      <assemblyIdentity
        type="win32"
        name="Microsoft.Windows.Common-Controls"
        version="6.0.0.0"
        processorArchitecture="*"
        publicKeyToken="6595b64144ccf1df"
        language="*"/>
    </dependentAssembly>
  </dependency>
</assembly>
```

### InitCommonControlsEx

**Initialization:**
```c
INITCOMMONCONTROLSEX icex;
icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
icex.dwICC = ICC_STANDARD_CLASSES;  /* Standard controls */
InitCommonControlsEx(&icex);
```

**Why is this needed?**
- Loads comctl32.dll v6 (themed version)
- Without it, you get v5 (non-themed)
- Must be called before creating any controls

**ICC flags:**
- `ICC_STANDARD_CLASSES` - Button, Edit, Static, ListBox, etc.
- `ICC_LISTVIEW_CLASSES` - ListView, Header
- `ICC_TREEVIEW_CLASSES` - TreeView
- `ICC_BAR_CLASSES` - Toolbar, Status bar
- `ICC_TAB_CLASSES` - Tab control

### ClearType Font Rendering

**CreateFont with ClearType:**
```c
HFONT hFont = CreateFont(
    -11, 0, 0, 0, FW_NORMAL,
    FALSE, FALSE, FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY,          /* Enable ClearType */
    DEFAULT_PITCH | FF_SWISS,
    "Segoe UI"
);
```

**Quality values:**
- `DEFAULT_QUALITY` - Standard rendering
- `PROOF_QUALITY` - High quality, slower
- `DRAFT_QUALITY` - Low quality, faster
- `ANTIALIASED_QUALITY` - Anti-aliased (grayscale)
- `CLEARTYPE_QUALITY` - ClearType (subpixel, best)

### Font Fallback

**Windows Vista+:**
- Segoe UI available
- Consolas available
- ClearType enabled by default

**Windows XP:**
- Falls back to Tahoma (similar to Segoe UI)
- Falls back to Courier New (Consolas not available)
- ClearType available but may not be enabled

**Font Selection Strategy:**
```c
/* Try modern font first, system will fallback if not available */
CreateFont(..., "Segoe UI");     /* Vista+ */
/* Windows automatically uses Tahoma on XP if Segoe UI not found */
```

## Common Issues

### Controls look old/flat (not themed)

**Causes:**
1. Manifest not embedded or not found
2. InitCommonControlsEx not called
3. Running on Windows 2000 or older

**Solution:**
- Verify pragma directive is present
- Call InitCommonControlsEx before creating controls
- Test on XP or newer

### Fonts look jagged

**Cause:** Not using CLEARTYPE_QUALITY

**Solution:**
```c
CreateFont(..., CLEARTYPE_QUALITY, ...);
```

### Segoe UI not rendering on XP

**This is expected!** Segoe UI was introduced in Windows Vista.

Windows will automatically use Tahoma (similar look) on XP.

## Testing Visual Styles

**To verify themes are working:**
1. Buttons should have gradient/hover effects
2. Group boxes should have themed borders
3. Edit controls should have blue focus rectangle
4. On Vista+, should use Segoe UI font

**Without themes:**
- Buttons are flat gray
- No hover effects
- No blue focus rectangles
- Falls back to old-style controls

## Compatibility

**Works on:**
- ✅ Windows XP (with Luna theme)
- ✅ Windows Vista/7 (Aero)
- ✅ Windows 8/8.1 (Modern UI)
- ✅ Windows 10 (Fluent Design)
- ✅ Windows 11 (Fluent Design)

**Does NOT work on:**
- ❌ Windows 2000 (no visual styles support)
- ❌ Windows 98/ME (too old)

## Stream Talking Points

1. **Why visual styles?** - Modern look, better UX, follows OS theme
2. **What is a manifest?** - Tells Windows which DLLs to use
3. **Why embedded manifest?** - Single .exe file, no external dependencies
4. **What is ClearType?** - Subpixel font rendering, much smoother
5. **Why Segoe UI?** - Modern Windows font, professional appearance
6. **What if font not available?** - Windows automatically falls back

## Next Steps (Hour 3)

- Add query input text box
- Add Execute button
- Keep the themed XP+ styling
- Display query results

## Comparison

**Hour 2 (Basic):**
- White background
- System font
- No styling

**Hour 2.1 (Windows 95):**
- Gray background
- MS Shell Dlg
- 3D borders
- Classic look

**Hour 2.2 (Windows XP+):**
- Gray background
- Segoe UI / Tahoma
- Themed controls
- Modern look
- ClearType fonts

Same layout, progressively better appearance!
