/**
 *  Re-group the FFT into more meaningful values by 
 *  splitting into one-third-octave bands,
 *  and by smoothing each point with its neighbors.
 *  
 *  Plot over time.
 */

var source, fft;

// height of fft == height/divisions
var divisions = 5;
var cnv;
var speed = 10;
var bins = 256;

function setup() {
  background(0);

  cnv = createCanvas(windowWidth, windowHeight);
  noFill();
  stroke(0,100);
  source = new p5.AudioIn();
  source.start();

  fft = new p5.FFT(0.6, bins);
  fft.setInput(source);
}

function draw() {
  touchStarted();
  var h = height/divisions;
  var spectrum = fft.analyze();
  var newBuffer = [];

  var scaledSpectrum = spectrum;
  var len = scaledSpectrum.length;

 
  // copy before clearing the background
  copy(cnv,0,0,width,height,0,speed,width,height);
  background(0, 0, 0, 1);

  // draw shape
  beginShape();

    // one at the far corner
    vertex(0, h);

    for (var i = 0; i < len; i++) {
      var amp = spectrum[i];
      var vol = source.getLevel();
      var col = map(vol, 0, 1, 255, 0);
      stroke(col);
      var x = map(i, 0, len-1, 0, width);
      var y = map(amp, 0, 255, h, 0);
      vertex(x, y);
    }

    // one last point at the end
    vertex(width, h);

  endShape();
}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 