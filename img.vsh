//attribute highp vec4 vertexIn;
//attribute highp vec2 textureIn;
//varying highp vec2 textureOut;

//void main(void)
//{
//    gl_Position = vertexIn;
//    textureOut = textureIn;
//}


attribute vec4 position;
attribute vec2 surfacePosAttrib;
varying vec2 surfacePosition;

void main() {
    surfacePosition = surfacePosAttrib;
    gl_Position = position;
}
