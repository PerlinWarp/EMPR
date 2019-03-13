var mic;
var thres = 0.11;

function setup() {
  createCanvas(windowWidth, windowHeight);
  background(0);
  //Get input from the mic
  mic = new p5.AudioIn();
  mic.start();
  //Fixing the permissions issue on chrome
  touchStarted();
}

function draw() {
  background(0);

  var vol = mic.getLevel();
  detectBeat(vol);

  //Draw a square in the middle of the screen
  rect(width/2-50,height/2-50,100,100);
}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 

function detectBeat(vol){
  if(vol > thres){
    backgroundColor = color( random(0,255), random(0,255), random(0,255) );
    background(backgroundColor);
    thres = thres * 1.1;
  }

}