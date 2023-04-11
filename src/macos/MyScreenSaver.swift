import Foundation
import ScreenSaver
import OpenGL.GL3

class MyScreenSaverView: ScreenSaverView {
    var openGLContext: NSOpenGLContext?
    var pixelFormat: NSOpenGLPixelFormat?

    required init?(coder: NSCoder) {
        super.init(coder: coder)
    }
    override init?(frame: NSRect, isPreview: Bool) {
        let attrs: [NSOpenGLPixelFormatAttribute] = [
            NSOpenGLPixelFormatAttribute(NSOpenGLPFAOpenGLProfile), NSOpenGLPixelFormatAttribute(NSOpenGLProfileVersion4_1Core),
            NSOpenGLPixelFormatAttribute(NSOpenGLPFADoubleBuffer),
            NSOpenGLPixelFormatAttribute(NSOpenGLPFAColorSize), NSOpenGLPixelFormatAttribute(24),
            0 // Terminateur de tableau
        ]
        
        let pixelFormat = NSOpenGLPixelFormat(attributes: attrs)
    
        super.init(frame: frame, isPreview: isPreview)
        
        if let pixelFormat = pixelFormat {
            let openGLContext = NSOpenGLContext(format: pixelFormat, share: nil)
            self.pixelFormat = pixelFormat
            self.openGLContext = openGLContext
        }

        cpp_init(Int32(frame.width), Int32(frame.height))
    }
    
    override func draw(_ rect: NSRect) {
        // Mettez votre code d'affichage 3D ici
    }
    
    override func animateOneFrame() {
        // Mettez votre code d'animation ici
        cpp_update()
        cpp_draw()
        setNeedsDisplay(bounds)
    }
    
    override func startAnimation() {
        super.startAnimation()
        // Mettez votre code de démarrage de l'animation ici
    }
    
    override func stopAnimation() {
        super.stopAnimation()
        // Mettez votre code d'arrêt de l'animation ici
        cpp_terminate()
    }
}
