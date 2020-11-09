from gnuradio import filter
from gnuradio import gr
from gnuradio.filter import firdes
import grgsm
from gnuradio import blocks

class control_decimator(gr.hier_block2):
    '''
    Make a Polyphase Filter decimator (complex in, complex out, floating-point taps)

    This simplifies the interface by allowing a single input stream to connect to this block.
    It will then output a stream that is the decimated output stream.
    '''
    def __init__(self, decim, taps=None, channel=0, atten=100,
                 use_fft_rotators=True, use_fft_filters=True):
	gr.hier_block2.__init__(self, "gsm_control_decimator",
				gr.io_signature(1, 1, gr.sizeof_gr_complex),
				gr.io_signature(1, 1, gr.sizeof_gr_complex))

        self._decim = decim
        self._channel = channel

        if (taps is not None) and (len(taps) > 0):
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(1, self._decim, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, self._decim)
        self.pfb = grgsm.gsm_control_decimator(self._decim, self._taps, self._channel,
                                            use_fft_rotators, use_fft_filters)

        self.connect(self, self.s2ss)

        for i in xrange(self._decim):
            self.connect((self.s2ss,i), (self.pfb,i))

        self.connect(self.pfb, self)

    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_channel(self, chan):
        self.pfb.set_channel(chan)

    def declare_sample_delay(self, delay):
        self.pfb.declare_sample_delay(delay)

