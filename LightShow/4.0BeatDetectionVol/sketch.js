var mic, fft;
// The minumum amplitude requires to trigger a wave
var thres = 0.11;
//A variable that stops it triggering all the time, decays over time
var cutOff = 0.11;

// We need to keep track of time for the decay
var framesSinceBeat = 0; 

//
var lastDetect = 0.2;

function setup() {
  createCanvas(windowWidth, windowHeight);
  backgroundColor = color( random(0,255), random(0,255), random(0,255) );
  background(0);
  // Get input from the mic
  mic = new p5.AudioIn();
  mic.start();
  // Fixing the permissions issue on chrome
  touchStarted();

}

function draw() {
  background(backgroundColor);

  var vol = mic.getLevel();
  detectBeat(vol);

  //Draw a square in the middle of the screen
  console.log(vol);
  var size = 50 + vol * 500;
  rect(width/2-size/2,height/2-size/2,size,size);
}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 

function detectBeat(vol){
  if(vol > thres && vol > cutOff){

    backgroundColor = color( random(0,255), random(0,255), random(0,255) );
    background(backgroundColor);
    fill(0);
    cutOff = vol * 1.2;
    framesSinceBeat = 0;
  }else{
    fill(255);
    if(framesSinceBeat <= 30){
      framesSinceBeat++;
    }else{
      cutOff *= 0.9;
      // We dont want to trigger a beat at very low volumes
      // Even if one has not been heard for a while
      cutOff = Math.max(thres, cutOff);
    }
  }

}
