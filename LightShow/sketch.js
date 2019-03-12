let mic;
var button;

var volHistory = [];

function setup() {
  createCanvas(640, 480);

  //Microphone setup
  mic = new p5.AudioIn();
  mic.start();
}

function draw() {
	background(0);
	var vol = mic.getLevel();
	console.log(vol);
	volHistory.push(vol);

	stroke(255);
	noFill();
	beginShape();
	for(var i = 0; i < volHistory.length; i++){
		var y = map(volHistory[i], 0, 1, height, 0);
		vertex(i,y);

	}
	endShape();

	if (volHistory.length > width){
		volHistory.splice(0,1);
	}

}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 