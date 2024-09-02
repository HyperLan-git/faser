#include "FreqResponseComponent.hpp"

FreqResponseComponent::FreqResponseComponent(
	const BiquadFilter& filter, juce::AudioParameterInt* filterParam,
	ResponseCallback&& callback)
	: filter(filter), filterParam(filterParam), callback(callback) {
	setSize(400, 400);
}

FreqResponseComponent::~FreqResponseComponent() { this->callback = nullptr; }

const double a = std::log10(MAX_FREQ), b = std::log10(40);

constexpr float MAX_DB = 40;

typedef std::complex<double> complex;

constexpr complex ONE(1.);

void FreqResponseComponent::paint(juce::Graphics& g) {
	using namespace std::complex_literals;

	BiquadFilterCoefficients c = filter.getCoeffs();
	int filters = *filterParam;
	int w = this->getWidth(), h = this->getHeight();
	if (w > MAX_WIDTH) w = MAX_WIDTH;
	if (w <= 1) return;
	if (c != this->lastCoeffs || filters != this->lastFilters) {
		complex a0(c.a0), a1(c.a1), a2(c.a2), b0(c.b0), b1(c.b1), b2(c.b2);

		for (int i = 0; i < w; i++) {
			double f = std::pow(10, i * (a - b) / (w - 1) + b);
			complex z = std::exp<double>(complex(
				0, juce::MathConstants<double>::pi * f / MAX_FREQ)),
				z1 = ONE / z, z2 = ONE / (z * z);

			values[i] = (b0 + b1 * z1 + b2 * z2) / (a0 + a1 * z1 + a2 * z2);
			if (filters > 1) values[i] = std::pow(values[i], filters);
		}
		this->lastFilters = filters;
		this->lastCoeffs = c;
	}

	g.fillAll(juce::Colours::black);

	juce::Path p, p2;

	p.preallocateSpace(1 + (w + 4) * 3);
	p.startNewSubPath({ -200, h / 2.f });

	p2.preallocateSpace(1 + (w + 4) * 3);
	p2.startNewSubPath({ -200, h / 2.f });
	for (int x = 0; x < w; x++) {
		if (values[x].real() == 0 && values[x].imag() == 0) {
			p.lineTo((float)x, h + 20.f);
			p2.lineTo((float)x, h + 20.f);
			continue;
		}
		const double mag = (this->filter.getType() == ALLPASS)
			? 0
			: 20 * std::log10(std::abs(values[x]));
		const double phase = std::arg(values[x]);
		const int y = (int)(h / 2 - h * mag / MAX_DB / 2);
		const int y2 =
			(int)(h / 2 - h * phase / juce::MathConstants<double>::pi / 2);
		p.lineTo((float)x, (float)y);
		if (y > 2 * h)
			p2.lineTo((float)x, (float)h + 20);
		else
			p2.lineTo((float)x, (float)y2);
	}
	p.lineTo((float)w + 200, h / 2.f);
	p.lineTo((float)w, (float)h + 20);
	p.lineTo((float)-200, (float)h + 20);
	p.closeSubPath();

	p2.lineTo((float)w + 200, (float)h / 2.f);
	p2.lineTo((float)w, (float)h + 20);
	p2.lineTo((float)-200, (float)h + 20);
	p2.closeSubPath();

	g.setColour(juce::Colours::orange);
	g.fillPath(p);
	g.setColour(juce::Colours::orange.darker());
	g.strokePath(p, juce::PathStrokeType(5),
		juce::AffineTransform().withAbsoluteTranslation(0, 3));

	g.setColour(juce::Colours::cyan.withAlpha(.1f));
	g.fillPath(p2);
	g.setColour(juce::Colours::cyan.darker().withAlpha(.1f));
	g.strokePath(p2, juce::PathStrokeType(5),
		juce::AffineTransform().withAbsoluteTranslation(0, 3));
}

float getFreq(float posX, int w) {
	return std::pow(10, (float)(posX * (a - b) / (w - 1) + b));
}

float getGain(float posY, int h) { return MAX_DB - posY * MAX_DB * 2 / h; }

void FreqResponseComponent::mouseDown(const juce::MouseEvent& event) {
	float freq = getFreq(event.position.x, this->getWidth()),
		gain = getGain(event.position.y, this->getHeight());
	auto cb = this->callback;
	if (cb)
		cb(std::make_optional<float>(freq),
			std::make_optional<float>(gain), std::optional<float>());
}
void FreqResponseComponent::mouseDrag(const juce::MouseEvent& event) {
	float freq = getFreq(event.position.x, this->getWidth()),
		gain = getGain(event.position.y, this->getHeight());
	auto callback = this->callback;
	if (callback)
		callback(std::make_optional<float>(freq),
			std::make_optional<float>(gain), std::optional<float>());
}

void FreqResponseComponent::mouseWheelMove(
	const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) {
	(void)event;
	auto callback = this->callback;
	if (callback)
		callback(std::optional<float>(), std::optional<float>(),
			std::make_optional<float>(wheel.isReversed ? -wheel.deltaY
				: wheel.deltaY));
}

void FreqResponseComponent::resized() {}