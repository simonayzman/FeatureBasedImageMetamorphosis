#Feature Based Image Metamorphosis#

by Peng Hui How, Josh Israel, and Simon Ayzman

See the final animations in the videos and animations directories, or on YouTube at: https://www.youtube.com/watch?v=GdLh_sE-e-o&list=PLjIR0u8ThmEVRRVTpQRo4wDKnG87FPNdU

###featureAdder

#####OVERVIEW

Just click anywhere to start drawing features. Keep in mind that the order that the features are drawn
for each respective image will be maintained for the output phase; e.g., the sixth feature drawn on the
source image (SI) will correlate to the sixth feature drawn on the destination image (DI), even if they 
were not meant to be correlated. That being said, the user can draw any number of times on either image 
without restriction. Five features can be drawn on the SI, followed by three on the DI, followed by two 
on the SI, followed by four on the DI. It is up to the user to keep track of the correct correlation of 
the features.

#####USAGE

Note the command line usage:<br>
	./featureAdder (source PNG image) (destination PNG image)<br>
				   [--es (source image input file name)]<br>
				   [--ed (destination image input file name)]<br>
				   [--os (source image output file name)]<br>
				   [--od (destination image output file name)]<br>
				   [--s  (screenshot image output file name)]<br>

The es/ed flags are for opening and editing feature files.<br>
The os/od flags are for base-naming the output feature files.<br>
The s flag is for base-naming the screenshot files.<br>

Note the program usage:<br>
At any point you may quit the program by pressing the 'q' key.<br>
At any point you may undo your last action by pressing the 'u' key. <br>
At any point you may produce a screenshot of your current window by pressing the 's' key. ***<br>
At any point you may produce two .feat output files for your features by pressing the 'o' key. ***<br>
At any point you may force-produce two .feat output files for your features by pressing the 'f' key. *** ^<br>

*** Note that you may do this multiple times in one session. Multiple versions will be created.<br>
^   Required when number of features specified on the SI and DI not match.<br>

###imageMorph

#####OVERVIEW

This program generates the intermediate frames between the two images.<br>

The number of frames generated is up to the user.<br>

The steepness of the time progression is also up to the user, there are two modes available for now:<br>
1) a truncated sine curve, which means the changes in the beginning and at the end are gradual, however the change in the center is steep.<br>
2) a linear line, i.e. constant rate of change<br>

Note that, at the moment, this only supports PNG files - we will assume this for simplicity, since pre-processing is easy.<br>

#####USAGE

Note the command line usage:<br>
	./imageMorph (source.feat) (target.feat) (source.png) (target.png)<br>
				 (intermediaryImagesBaseName) (numFrames) (easingFunction)<br>

The easing function is either linear or sine, indicated by '-l' and '-s' respectively.<br>

### Final Animation

In order to produce the final image, there is a lot of work that went into the actual production of the 
intermediate images necessary. Doing it will take very many animator-hours. The two original videos (given)
were broken down into frames (not given). Then, specific keyframes were carefully selected (not given). The
featureAdder tool was then used on each respective pair of keyframes to specify common features. Two .feat
files was made for each image pair (given). Then, imageMorph was run on all the images and their respective
.feat files. The frames in each morph were then carefully selected to be added to the final set of animation
frames (given). Finally, these frames were all combined into a single video (given, of course).
