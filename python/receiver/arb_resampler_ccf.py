from gnuradio import filter
from gnuradio import gr
from gnuradio.filter import firdes
import grgsm
from gnuradio import blocks
from gnuradio.filter import filter_swig as filter
import math, cmath

class arb_resampler_ccf(gr.hier_block2):
    '''
    Convenience wrapper for the polyphase filterbank arbitrary resampler.

    The block takes a single complex stream in and outputs a single complex
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, rate, taps=None, flt_size=32, atten=100):
        gr.hier_block2.__init__(self, "control_arb_resampler_ccf",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._rate = rate
        self._size = flt_size
        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the output signal

            # If rate >= 1, we need to prevent images in the output,
            # so we have to filter it to less than half the channel
            # width of 0.5.  If rate < 1, we need to filter to less
            # than half the output signal's bw to avoid aliasing, so
            # the half-band here is 0.5*rate.
            percent = 0.80
            if(self._rate < 1):
                halfband = 0.5*self._rate
                bw = percent*halfband
                tb = (percent/2.0)*halfband
                ripple = 0.1

                # As we drop the bw factor, the optfir filter has a harder time converging;
                # using the firdes method here for better results.
                self._taps = filter.firdes.low_pass_2(self._size, self._size, bw, tb, atten,
                                                      filter.firdes.WIN_BLACKMAN_HARRIS)
            else:
                halfband = 0.5
                bw = percent*halfband
                tb = (percent/2.0)*halfband
                ripple = 0.1

                made = False
                while not made:
                    try:
                        self._taps = self.low_pass_filter(self._size, self._size, bw, bw+tb, ripple, atten)
                        made = True
                    except RuntimeError:
                        ripple += 0.01
                        made = False
                        print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                        # Build in an exit strategy; if we've come this far, it ain't working.
                        if(ripple >= 1.0):
                            raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = grgsm.control_arb_resampler_ccf(self._rate, self._taps, self._size)
        #print "PFB has %d taps\n" % (len(self._taps),)

        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    # Note -- set_taps not implemented in base class yet
    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_rate(self, rate):
        self.pfb.set_rate(rate)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)

    def low_pass_filter (self, gain, Fs, freq1, freq2, passband_ripple_db, stopband_atten_db,
                  nextra_taps=2):
        """
        Builds a low pass filter.

        Args:
            gain: Filter gain in the passband (linear)
            Fs: Sampling rate (sps)
            freq1: End of pass band (in Hz)
            freq2: Start of stop band (in Hz)
            passband_ripple_db: Pass band ripple in dB (should be small, < 1)
            stopband_atten_db: Stop band attenuation in dB (should be large, >= 60)
            nextra_taps: Extra taps to use in the filter (default=2)
        """
        passband_dev = self.passband_ripple_to_dev (passband_ripple_db)
        
        stopband_dev = self.stopband_atten_to_dev (stopband_atten_db)
        desired_ampls = (gain, 0)
        (n, fo, ao, w) = self.remezord ([freq1, freq2], desired_ampls,
                               [passband_dev, stopband_dev], Fs)
        # The remezord typically under-estimates the filter order, so add 2 taps by default
        taps = filter.pm_remez (n + nextra_taps, fo, ao, w, "bandpass")
        return taps

    def passband_ripple_to_dev (self, ripple_db):
        """Convert passband ripple spec expressed in dB to an absolute value"""
        return (10**(ripple_db/20)-1)/(10**(ripple_db/20)+1)

    def stopband_atten_to_dev(self, atten_db):
        """Convert a stopband attenuation in dB to an absolute value"""
        return 10 ** (-atten_db / 20)

    def remezord(self, fcuts, mags, devs, fsamp=2):
        '''
        FIR order estimator (lowpass, highpass, bandpass, mulitiband).

        (n, fo, ao, w) = remezord (f, a, dev)
        (n, fo, ao, w) = remezord (f, a, dev, fs)

        (n, fo, ao, w) = remezord (f, a, dev) finds the approximate order,
        normalized frequency band edges, frequency band amplitudes, and
        weights that meet input specifications f, a, and dev, to use with
        the remez command.

        * f is a sequence of frequency band edges (between 0 and Fs/2, where
          Fs is the sampling frequency), and a is a sequence specifying the
          desired amplitude on the bands defined by f. The length of f is
          twice the length of a, minus 2. The desired function is
          piecewise constant.

        * dev is a sequence the same size as a that specifies the maximum
          allowable deviation or ripples between the frequency response
          and the desired amplitude of the output filter, for each band.

        Use remez with the resulting order n, frequency sequence fo,
        amplitude response sequence ao, and weights w to design the filter b
        which approximately meets the specifications given by remezord
        input parameters f, a, and dev:

        b = remez (n, fo, ao, w)

        (n, fo, ao, w) = remezord (f, a, dev, Fs) specifies a sampling frequency Fs.

        Fs defaults to 2 Hz, implying a Nyquist frequency of 1 Hz. You can
        therefore specify band edges scaled to a particular applications
        sampling frequency.

        In some cases remezord underestimates the order n. If the filter
        does not meet the specifications, try a higher order such as n+1
        or n+2.
        '''
        # get local copies
        fcuts = fcuts[:]
        mags = mags[:]
        devs = devs[:]

        for i in range(len(fcuts)):
            fcuts[i] = float(fcuts[i]) / fsamp

        nf = len(fcuts)
        nm = len(mags)
        nd = len(devs)
        nbands = nm

        if nm != nd:
            raise ValueError, "Length of mags and devs must be equal"

        if nf != 2 * (nbands - 1):
            raise ValueError, "Length of f must be 2 * len (mags) - 2"

        for i in range(len(mags)):
            if mags[i] != 0:  # if not stopband, get relative deviation
                devs[i] = devs[i] / mags[i]

        # separate the passband and stopband edges
        f1 = fcuts[0::2]
        f2 = fcuts[1::2]

        n = 0
        min_delta = 2
        for i in range(len(f1)):
            if f2[i] - f1[i] < min_delta:
                n = i
                min_delta = f2[i] - f1[i]

        if nbands == 2:
            # lowpass or highpass case (use formula)
            l = self.lporder(f1[n], f2[n], devs[0], devs[1])
        else:
            # bandpass or multipass case
            # try different lowpasses and take the worst one that
            #  goes through the BP specs
            l = 0
            for i in range(1, nbands - 1):
                l1 = self.lporder(f1[i - 1], f2[i - 1], devs[i], devs[i - 1])
                l2 = self.lporder(f1[i], f2[i], devs[i], devs[i + 1])
                l = max(l, l1, l2)

        n = int(math.ceil(l)) - 1  # need order, not length for remez

        # cook up remez compatible result
        ff = [0] + fcuts + [1]
        for i in range(1, len(ff) - 1):
            ff[i] *= 2

        aa = []
        for a in mags:
            aa = aa + [a, a]

        max_dev = max(devs)
        wts = [1] * len(devs)
        for i in range(len(wts)):
            wts[i] = max_dev / devs[i]

        return (n, ff, aa, wts)

    def lporder (self, freq1, freq2, delta_p, delta_s):
        '''
        FIR lowpass filter length estimator.  freq1 and freq2 are
        normalized to the sampling frequency.  delta_p is the passband
        deviation (ripple), delta_s is the stopband deviation (ripple).

        Note, this works for high pass filters too (freq1 > freq2), but
        doesnt work well if the transition is near f == 0 or f == fs/2

        From Herrmann et al (1973), Practical design rules for optimum
        finite impulse response filters.  Bell System Technical J., 52, 769-99
        '''
        df = abs (freq2 - freq1)
        ddp = math.log10 (delta_p)
        dds = math.log10 (delta_s)

        a1 = 5.309e-3
        a2 = 7.114e-2
        a3 = -4.761e-1
        a4 = -2.66e-3
        a5 = -5.941e-1
        a6 = -4.278e-1

        b1 = 11.01217
        b2 = 0.5124401

        t1 = a1 * ddp * ddp
        t2 = a2 * ddp
        t3 = a4 * ddp * ddp
        t4 = a5 * ddp

        dinf=((t1 + t2 + a3) * dds) + (t3 + t4 + a6)
        ff = b1 + b2 * (ddp - dds)
        n = dinf / df - ff * df + 1
        return n

