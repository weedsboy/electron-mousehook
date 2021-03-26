# electron-mousehook

Electron mousehook for win.

You can receive mouse events in Windows system.

Must be called on the main thread.

## Usage

```js
//start MouseHook
const mousehook = require('electron-mousehook')
mousehook.StartMouseHook(wnd)

win.hookWindowMessage(result, async (wParam, lParam) => {
    const event = wParam.readUInt32LE(0)
    if (event == 0x0200 || event == 0x00a0) {
        console.log('mouse move')
    } else if (event == 0x0201 || event == 0x00a1) {
        console.log('L down')
    } else if (event == 0x0202 || event == 0x00a2) {
        console.log('L up')
    }
})

//stop MouseHook
mousehook.StopMouseHook()

```

## License

MIT, please see LICENSE for details. Copyright (c) 2021 weedsboy.
