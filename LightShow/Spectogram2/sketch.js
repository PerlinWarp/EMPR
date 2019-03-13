var mic,fft;

// Our canvas object, used for copying
var cnv;
var len;

function setup() {
  createCanvas(windowWidth, windowHeight);
  colorMode(HSB);
  background(0);
  noStroke();
  // Get input from the mic
  mic = new p5.AudioIn();
  mic.start();
  fft = new p5.FFT(0.6);
  fft.setInput(mic);

  // Fixing the permissions issue on chrome
  touchStarted();

  len = windowHeight / 1024;
}

function draw() {
  //background(backgroundColor);

  var spectrum = fft.analyze();
  console.log(spectrum);

  for (var i = 0; i < spectrum.length; i++){
    var val = spectrum[i];
    fill(val, 255, val);
    rect(width - 100, i*len, 100, (i+1)*len);
  }
}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 

