var mgpFontContext;
function getFontContext() {
  if (!mgpFontContext) {
    var canvas = document.createElement("canvas")
    canvas.width = 512;
    canvas.height = 512;
    mgpFontContext = canvas.getContext("2d", { willReadFrequently: true });
  }
  //mgpFontContext.textBaseline = "top";
  return mgpFontContext;
}

function fixGlfw(canvas) {
    var _addEventListener = canvas.addEventListener;
    canvas.addEventListener = function(type, listner, options) {
        if (type === 'touchstart' || type === 'touchend' || type === 'touchmove' || type == 'wheel' || type == 'mousewheel') {
            var preventDefault = false;
            if (type == "touchstart") {
                preventDefault = true;
                options = {passive: false};
            }

            _addEventListener(type, function (ev) {
                const handler = {
                    get: function (obj, prop) {
                        if (prop == "type") {
                            let etype = obj[prop];
                            if (etype == "touchstart") {
                                return "mousedown";
                            }
                            else if (etype == "touchend") {
                                return "mousedown";
                            }
                            else if (etype == "touchmove") {
                                return "mousemove";
                            }
                            return etype;
                        }
                        else if (prop == "preventDefault") {
                            return function(){};
                        }
                        return obj[prop];
                    },
                };
                
                const p = new Proxy(ev, handler);
                listner(p);

                if (preventDefault) {
                    ev.preventDefault();
                }
                //ev.touche = ev.touches[0];
            }, options);
        }
        else {
            _addEventListener(type, listner, options);
        }
    }
}