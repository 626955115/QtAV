#include "QmlAV/QuickFilter.h"
#include "QtAV/private/Filter_p.h"
#include "QtAV/LibAVFilter.h"
#include "QtAV/GLSLFilter.h"
#include "QtAV/VideoShaderObject.h"
#include "QtAV/OpenGLVideo.h"

//namespace QtAV {

class QuickVideoFilterPrivate : public VideoFilterPrivate
{
public:
    QuickVideoFilterPrivate() : type(QuickVideoFilter::AVFilter)
      , avfilter(new LibAVFilterVideo())
      , glslfilter(new GLSLFilter())
      , shader(new DynamicShaderObject())
    {
        glslfilter->opengl()->setUserShader(shader.data());
        filter = avfilter.data();
    }

    QuickVideoFilter::FilterType type;
    VideoFilter *filter;
    QScopedPointer<VideoFilter> user_filter;
    QScopedPointer<LibAVFilterVideo> avfilter;
    QScopedPointer<GLSLFilter> glslfilter;
    QScopedPointer<DynamicShaderObject> shader;
};

QuickVideoFilter::QuickVideoFilter(QObject *parent)
    : VideoFilter(*new QuickVideoFilterPrivate(), parent)
{
    DPTR_D(QuickVideoFilter);
    connect(d.shader.data(), SIGNAL(headerChanged()), this, SIGNAL(fragHeaderChanged()));
    connect(d.shader.data(), SIGNAL(sampleChanged()), this, SIGNAL(fragSampleChanged()));
    connect(d.shader.data(), SIGNAL(postProcessChanged()), this, SIGNAL(fragPostProcessChanged()));
    connect(d.avfilter.data(), SIGNAL(optionsChanged()), this, SIGNAL(avfilterChanged()));
}

bool QuickVideoFilter::isSupported(VideoFilterContext::Type ct) const
{
    DPTR_D(const QuickVideoFilter);
    if (d.filter)
        return d.filter->isSupported(ct);
    return false;
}

QuickVideoFilter::FilterType QuickVideoFilter::type() const
{
    return d_func().type;
}

void QuickVideoFilter::setType(FilterType value)
{
    DPTR_D(QuickVideoFilter);
    if (d.type == value)
        return;
    d.type = value;
    if (value == GLSLFilter)
        d.filter = d.glslfilter.data();
    else if (value == AVFilter)
        d.filter = d.avfilter.data();
    else
        d.filter = d.user_filter.data();
    Q_EMIT typeChanged();
}

QStringList QuickVideoFilter::supportedAVFilters() const
{
    return d_func().avfilter->filters();
}

QString QuickVideoFilter::avfilter() const
{
    return d_func().avfilter->options();
}

void QuickVideoFilter::setAVFilter(const QString &options)
{
    d_func().avfilter->setOptions(options);
}

VideoFilter* QuickVideoFilter::userFilter() const
{
    return d_func().user_filter.data();
}

void QuickVideoFilter::setUserFilter(VideoFilter *f)
{
    DPTR_D(QuickVideoFilter);
    if (d.user_filter.data() == f)
        return;
    d.user_filter.reset(f);
    Q_EMIT userFilterChanged();
}

QString QuickVideoFilter::fragHeader() const
{
    return d_func().shader->header();
}

void QuickVideoFilter::setFragHeader(const QString &c)
{
    d_func().shader->setHeader(c);
}

QString QuickVideoFilter::fragSample() const
{
    return d_func().shader->sample();
}

void QuickVideoFilter::setFragSample(const QString& c)
{
    return d_func().shader->setSample(c);
}

QString QuickVideoFilter::fragPostProcess() const
{
    return d_func().shader->postProcess();
}

void QuickVideoFilter::setFragPostProcess(const QString& c)
{
    d_func().shader->setPostProcess(c);
}

void QuickVideoFilter::process(Statistics *statistics, VideoFrame *frame)
{
    DPTR_D(QuickVideoFilter);
    if (!d.filter)
        return;
    d.filter->apply(statistics, frame);
}

class QuickAudioFilterPrivate : public AudioFilterPrivate
{
public:
    QuickAudioFilterPrivate() : AudioFilterPrivate()
      , avfilter(new LibAVFilterAudio())
    {
        filter = avfilter.data();
    }

    AudioFilter *filter;
    QScopedPointer<AudioFilter> user_filter;
    QScopedPointer<LibAVFilterAudio> avfilter;
};

QuickAudioFilter::QuickAudioFilter(QObject *parent)
    : AudioFilter(*new QuickAudioFilterPrivate(), parent)
{
    DPTR_D(QuickAudioFilter);
    connect(d.avfilter.data(), SIGNAL(optionsChanged()), this, SIGNAL(avfilterChanged()));
}

QStringList QuickAudioFilter::supportedAVFilters() const
{
    return d_func().avfilter->filters();
}

QString QuickAudioFilter::avfilter() const
{
    return d_func().avfilter->options();
}

void QuickAudioFilter::setAVFilter(const QString &options)
{
    d_func().avfilter->setOptions(options);
}

AudioFilter *QuickAudioFilter::userFilter() const
{
    return d_func().user_filter.data();
}

void QuickAudioFilter::setUserFilter(AudioFilter *f)
{
    DPTR_D(QuickAudioFilter);
    if (d.user_filter.data() == f)
        return;
    d.user_filter.reset(f);
    Q_EMIT userFilterChanged();
}

void QuickAudioFilter::process(Statistics *statistics, AudioFrame *frame)
{
    DPTR_D(QuickAudioFilter);
    if (!d.filter)
        return;
    d.filter->apply(statistics, frame);
}
//} //namespace QtAV
