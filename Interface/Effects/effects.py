import numpy as np
import wave
# I ONLY TESTED THIS WITH AUDACITY'S 16BIT SIGNED WAVs AT 16KHz
# at some point i'll make it work with any bitrate
# but the 16bit signed thing is here to stay afaik
class EffectsConsole():
    # effects will be applied to file at sourcePath
    # they are stored in memory until you call save()
    def __init__(self, sourcePath):
        self.sourcePath = sourcePath
        sourceFile = open(sourcePath, 'rb')
        wf = wave.open(sourceFile)
        self.samplerate = wf.getframerate()
        wf.close()
        sourceFile = open(sourcePath, 'rb')
        self.header = sourceFile.read(44)
        self.samples = []
        allData = sourceFile.read()
        sourceFile.close()
        # assuming 16bit samples
        for i in range(0, len(allData) - 1, 2):
            self.samples.append(np.int16((allData[i + 1] << 8) | allData[i]))
        self.samples = np.array(self.samples, dtype='int16')

    def msToSmpl(self, ms):
        return int(self.samplerate * (ms / 1000))
    def __applyConv(self, conv):
        self.samples = np.convolve(self.samples, conv, mode='same')
        self.samples = self.samples.astype(dtype='int16')
    # n is length of convolution averaging array
    # 20 is usually an ok choice

    # apply a low pass filter (1d-convolution of size n, with a homemade function that drops off rapidly)
    # work pretty nice for removing noise
    def effectFilter(self, n):
        n = self.msToSmpl(n)
        l = np.array([*np.linspace(0, 1, n // 2), *np.linspace(1, 0, n // 2)[1:]]) ** 3
        l /= sum(l)

        self.__applyConv(l)

    # add a delay - add sample i - n to the ith sample
    def effectDelay(self, n):
        n = self.msToSmpl(n)
        x1 = [0.75]
        x2 = np.zeros(n)
        x3 = [0.75]
        l = np.array([*x1, *x2, *x3])

        self.__applyConv(l)

    # this one is amazing
    # it slowly varies the amount of delay between 2 thresholds (i - l and i - r)
    # sounds v.v. spooky/funky/noisy depending on the settings
    def effectModAmpl(self, period, intensity):
        print(self.msToSmpl(period))
        modulator = np.sin(np.linspace(0, 2 * np.pi, self.msToSmpl(period)))

        for i in range(len(self.samples)):
            self.samples[i] = self.samples[i] * (1 - intensity) \
                              + self.samples[i] * modulator[i % len(modulator)] * intensity

    # this one is amazing
    # it slowly varies the amount of delay between 2 thresholds (i - l and i - r)
    # sounds v.v. spooky/funky/noisy depending on the settings
    def effectFlange(self, l, r):
        l, r = self.msToSmpl(l), self.msToSmpl(r)
        i = l
        j,jdelta = 0, 10

        while i < len(self.samples) - r:
            self.samples[i] = self.samples[i] * 0.9 + self.samples[j] * 0.1
            j += jdelta
            if j < i - l:
                j = i - l
                jdelta = -jdelta
            if j > i:
                j = i
                jdelta = -jdelta
            i += 1

    # the results will be save at targetPath
    # the internal buffer is kept - you can keep applying more effects
    def save(self, targetPath):
        targetFile = open(targetPath, 'wb')
        targetFile.write(self.header)
        targetFile.write(self.samples.tobytes())
        targetFile.close()

# example usage:
# ec = EffectsConsole('test.wav')
# ec.effectModAmpl(1000, 1)
# ec.effectFilter(20)
# ec.effectDelay(10000)
# ec.effectFlange(2000, 1000)
# ec.save('out2.wav')