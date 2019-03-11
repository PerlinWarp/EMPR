let mic;

function setup() {
  createCanvas(640, 480);

  //Microphone setup
  mic = new p5.AudioIn();
  mic.start();
}

function draw() {
	var vol = mic.getLevel();
	console.log(vol);
}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 
