mergeInto(LibraryManager.library, {
    fontRender: function(uniChar, fontName, bold, fontSize, out) {
      //console.log("fontRender");
      var cx = getFontContext();
      cx.font = bold?"bold ":""+fontSize + "px Arial";
  
      if (uniChar) {
        var text = String.fromCodePoint(uniChar);
      }
      else {
        text = "W";
      }
  
      var m = cx.measureText(text);
      var w = m.width;
      var h = m.fontBoundingBoxDescent + m.fontBoundingBoxAscent;
      if (w == 0) w = 1;
      var p = out >> 2;
      Module.HEAPF32[p++] = w+1;
      Module.HEAPF32[p++] = w;
      Module.HEAPF32[p++] = h;
      Module.HEAPF32[p++] = 0;
      Module.HEAPF32[p++] = m.fontBoundingBoxAscent;
  
      var iw = Math.ceil(w);
      var ih = Math.ceil(h);
      cx.clearRect(0, 0, iw, ih);
      cx.fillStyle = "rgba(255,0,0,1)";
      cx.fillText(text, 0, m.fontBoundingBoxAscent);
      var imageData = cx.getImageData(0, 0, iw, ih);
      var imageBuf = imageData.data;
  
      var buf = Module._malloc(imageBuf.length*imageBuf.BYTES_PER_ELEMENT);
      Module.HEAPU8.set(imageBuf, buf);
      return buf;
    },
    fontMerics: function(uniChar, fontName, bold, fontSize, out) {
      //console.log("fontMerics");
      var cx = getFontContext();
      cx.font = bold?"bold ":""+fontSize + "px Arial";
  
      if (uniChar) {
        var text = String.fromCodePoint(uniChar);
      }
      else {
        text = "W";
      }
  
      var m = cx.measureText(text);
      var w = m.width;
      var h = m.fontBoundingBoxDescent + m.fontBoundingBoxAscent;
      if (w == 0) w = 1;
      var p = out >> 2;
      Module.HEAPF32[p++] = w+1;
      Module.HEAPF32[p++] = w;
      Module.HEAPF32[p++] = h;
      Module.HEAPF32[p++] = 0;
      Module.HEAPF32[p++] = m.fontBoundingBoxAscent;
      return 1;
    },
    fontKerning: function(uniChar, fontName, bold, fontSize, previous) {
      var cx = getFontContext();
      cx.font = bold?"bold ":""+fontSize + "px Arial";
  
      var t1 = String.fromCodePoint(previous);
      var t2 = String.fromCharCode(uniChar);
      var m = cx.measureText(t1+t2).width;
      var m1 = cx.measureText(t1).width;
      var m2 = cx.measureText(t2).width;
  
      var res = m - (m1 + m2);
      return res;
    },
  
    editTextCreate: function(inputType, text, textBox) {
      var elem = this._editText;
      if (elem) {
        elem.parentNode.removeChild(elem);
      }
  
      var field = null;
      if (inputType == 2) {
        field = document.createElement("input");
        field.type = "password";
      }
      else if (inputType == 3) {
        field = document.createElement("textarea");
        field.rows = 2;
      }
      else {
        field = document.createElement("input");
        field.type = "text";
      }
  
      field.value = Module.UTF8ToString(text);
  
      field.style.position = "absolute";
      field.style.border = "0";
      field.style.outline = "0";
  
      field.addEventListener("input", function() {
        //view.textChange(field.value);
        Module.ccall('editTextOnTextChange', null, ["number", "string"], 
              [textBox, field.value]);
      });
  
      var canvas = Module["canvas"];
      //canvas.setAttribute('tabindex','0');
      var parentNode = canvas.parentNode;
      parentNode.appendChild(field);
      this._editText = field;
  
      field.focus();
      if (GLFW) {
        window.removeEventListener("keydown", GLFW.onKeydown, true);
      }
    },
    editTextRemove: function() {
      var elem = this._editText;
      if (!elem) return;
      elem.parentNode.removeChild(elem);
      this._editText = null;
      if (GLFW) {
        window.addEventListener("keydown", GLFW.onKeydown, true);
      }
    },
    editTextUpdate: function(x, y, w, h, fontSize) {
      var elem = this._editText;
      if (!elem) return;
  
      var canvas = Module["canvas"];
  
      elem.style.left = (canvas.offsetLeft + x) + "px";
      elem.style.top = (canvas.offsetTop + y) + "px";
      elem.style.width = w + "px";
      elem.style.height = h + "px";
      elem.style.margin = 0;
      elem.style.padding = 0;
      elem.style["font-size"] = fontSize + "px";
    }
});