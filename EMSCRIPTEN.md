## Building for the web (Emscripten)
The browser build uses the Emscripten SDK (tested with emcc 6.0). It does not change the native build: the SDL2 libraries come from Emscripten's ports and PhysFS is fetched and built from source. The first configure therefore needs network access. To build offline pass `-DFETCHCONTENT_SOURCE_DIR_PHYSFS=<path to a physfs release-3.2.0 checkout>`.
```bash
source <emsdk>/emsdk_env.sh
./packdata.sh
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```
The result is `build-web/web/index.html` with `index.js`, `index.wasm` and `index.data`. Browsers refuse to load wasm from `file://`, so serve the folder:
```bash
python3 -m http.server -d build-web/web 8080
```
and open <http://localhost:8080/>.

Settings and highscores are stored in the browser's IndexedDB for that origin. Command line options, the editors and screenshots are not available in the browser.
