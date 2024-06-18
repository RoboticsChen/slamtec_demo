/* boost random/exponential_distribution.hpp header file
 *
 * Copyright Jens Maurer 2000-2001
 * Copyright Steven Watanabe 2011
 * Copyright Jason Rhinelander 2016
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id$
 *
 * Revision history
 *  2001-02-18  moved to individual header files
 */

#ifndef BOOST_RANDOM_EXPONENTIAL_DISTRIBUTION_HPP
#define BOOST_RANDOM_EXPONENTIAL_DISTRIBUTION_HPP

#include <boost/config/no_tr1/cmath.hpp>
#include <iosfwd>
#include <boost/assert.hpp>
#include <boost/limits.hpp>
#include <boost/random/detail/config.hpp>
#include <boost/random/detail/operators.hpp>
#include <boost/random/detail/int_float_pair.hpp>
#include <boost/random/uniform_01.hpp>

namespace boost {
namespace random {

namespace detail {

// tables for the ziggurat algorithm
template<class RealType>
struct exponential_table {
    static const RealType table_x[257];
    static const RealType table_y[257];
};

template<class RealType>
const RealType exponential_table<RealType>::table_x[257] = {
    8.6971174701310497140, 7.6971174701310497140, 6.9410336293772123602, 6.4783784938325698538,
    6.1441646657724730491, 5.8821443157953997963, 5.6664101674540337371, 5.4828906275260628694,
    5.3230905057543986131, 5.1814872813015010392, 5.0542884899813047117, 4.9387770859012514838,
    4.8329397410251125881, 4.7352429966017412526, 4.6444918854200854873, 4.5597370617073515513,
    4.4802117465284221949, 4.4052876934735729805, 4.3344436803172730116, 4.2672424802773661873,
    4.2033137137351843802, 4.1423408656640511251, 4.0840513104082974638, 4.0282085446479365106,
    3.9746060666737884793, 3.9230625001354895926, 3.8734176703995089983, 3.8255294185223367372,
    3.7792709924116678992, 3.7345288940397975350, 3.6912010902374189454, 3.6491955157608538478,
    3.6084288131289096339, 3.5688252656483374051, 3.5303158891293438633, 3.4928376547740601814,
    3.4563328211327607625, 3.4207483572511205323, 3.3860354424603017887, 3.3521490309001100106,
    3.3190474709707487166, 3.2866921715990692095, 3.2550473085704501813, 3.2240795652862645207,
    3.1937579032122407483, 3.1640533580259734580, 3.1349388580844407393, 3.1063890623398246660,
    3.0783802152540905188, 3.0508900166154554479, 3.0238975044556767713, 2.9973829495161306949,
    2.9713277599210896472, 2.9457143948950456386, 2.9205262865127406647, 2.8957477686001416838,
    2.8713640120155362592, 2.8473609656351888266, 2.8237253024500354905, 2.8004443702507381944,
    2.7775061464397572041, 2.7548991965623453650, 2.7326126361947007411, 2.7106360958679293686,
    2.6889596887418041593, 2.6675739807732670816, 2.6464699631518093905, 2.6256390267977886123,
    2.6050729387408355373, 2.5847638202141406911, 2.5647041263169053687, 2.5448866271118700928,
    2.5253043900378279427, 2.5059507635285939648, 2.4868193617402096807, 2.4679040502973649846,
    2.4491989329782498908, 2.4306983392644199088, 2.4123968126888708336, 2.3942890999214583288,
    2.3763701405361408194, 2.3586350574093374601, 2.3410791477030346875, 2.3236978743901964559,
    2.3064868582835798692, 2.2894418705322694265, 2.2725588255531546952, 2.2558337743672190441,
    2.2392628983129087111, 2.2228425031110364013, 2.2065690132576635755, 2.1904389667232199235,
    2.1744490099377744673, 2.1585958930438856781, 2.1428764653998416425, 2.1272876713173679737,
    2.1118265460190418108, 2.0964902118017147637, 2.0812758743932248696, 2.0661808194905755036,
    2.0512024094685848641, 2.0363380802487695916, 2.0215853383189260770, 2.0069417578945183144,
    1.9924049782135764992, 1.9779727009573602295, 1.9636426877895480401, 1.9494127580071845659,
    1.9352807862970511135, 1.9212447005915276767, 1.9073024800183871196, 1.8934521529393077332,
    1.8796917950722108462, 1.8660195276928275962, 1.8524335159111751661, 1.8389319670188793980,
    1.8255131289035192212, 1.8121752885263901413, 1.7989167704602903934, 1.7857359354841254047,
    1.7726311792313049959, 1.7596009308890742369, 1.7466436519460739352, 1.7337578349855711926,
    1.7209420025219350428, 1.7081947058780575683, 1.6955145241015377061, 1.6829000629175537544,
    1.6703499537164519163, 1.6578628525741725325, 1.6454374393037234057, 1.6330724165359912048,
    1.6207665088282577216, 1.6085184617988580769, 1.5963270412864831349, 1.5841910325326886695,
    1.5721092393862294810, 1.5600804835278879161, 1.5481036037145133070, 1.5361774550410318943,
    1.5243009082192260050, 1.5124728488721167573, 1.5006921768428164936, 1.4889578055167456003,
    1.4772686611561334579, 1.4656236822457450411, 1.4540218188487932264, 1.4424620319720121876,
    1.4309432929388794104, 1.4194645827699828254, 1.4080248915695353509, 1.3966232179170417110,
    1.3852585682631217189, 1.3739299563284902176, 1.3626364025050864742, 1.3513769332583349176,
    1.3401505805295045843, 1.3289563811371163220, 1.3177933761763245480, 1.3066606104151739482,
    1.2955571316866007210, 1.2844819902750125450, 1.2734342382962410994, 1.2624129290696153434,
    1.2514171164808525098, 1.2404458543344064544, 1.2294981956938491599, 1.2185731922087903071,
    1.2076698934267612830, 1.1967873460884031665, 1.1859245934042023557, 1.1750806743109117687,
    1.1642546227056790397, 1.1534454666557748056, 1.1426522275816728928, 1.1318739194110786733,
    1.1211095477013306083, 1.1103581087274114281, 1.0996185885325976575, 1.0888899619385472598,
    1.0781711915113727024, 1.0674612264799681530, 1.0567590016025518414, 1.0460634359770445503,
    1.0353734317905289496, 1.0246878730026178052, 1.0140056239570971074, 1.0033255279156973717,
    0.99264640550727647009, 0.98196705308506317914, 0.97128624098390397896, 0.96060271166866709917,
    0.94991517776407659940, 0.93922231995526297952, 0.92852278474721113999, 0.91781518207004493915,
    0.90709808271569100600, 0.89637001558989069006, 0.88562946476175228052, 0.87487486629102585352,
    0.86410460481100519511, 0.85331700984237406386, 0.84251035181036928333, 0.83168283773427388393,
    0.82083260655441252290, 0.80995772405741906620, 0.79905617735548788109, 0.78812586886949324977,
    0.77716460975913043936, 0.76617011273543541328, 0.75513998418198289808, 0.74407171550050873971,
    0.73296267358436604916, 0.72181009030875689912, 0.71061105090965570413, 0.69936248110323266174,
    0.68806113277374858613, 0.67670356802952337911, 0.66528614139267855405, 0.65380497984766565353,
    0.64225596042453703448, 0.63063468493349100113, 0.61893645139487678178, 0.60715622162030085137,
    0.59528858429150359384, 0.58332771274877027785, 0.57126731653258903915, 0.55910058551154127652,
    0.54682012516331112550, 0.53441788123716615385, 0.52188505159213564105, 0.50921198244365495319,
    0.49638804551867159754, 0.48340149165346224782, 0.47023927508216945338, 0.45688684093142071279,
    0.44332786607355296305, 0.42954394022541129589, 0.41551416960035700100, 0.40121467889627836229,
    0.38661797794112021568, 0.37169214532991786118, 0.35639976025839443721, 0.34069648106484979674,
    0.32452911701691008547, 0.30783295467493287307, 0.29052795549123115167, 0.27251318547846547924,
    0.25365836338591284433, 0.23379048305967553619, 0.21267151063096745264, 0.18995868962243277774,
    0.16512762256418831796, 0.13730498094001380420, 0.10483850756582017915, 0.063852163815003480173,
    0
};

template<class RealType>
const RealType exponential_table<RealType>::table_y[257] = {
    0, 0.00045413435384149675545, 0.00096726928232717452884, 0.0015362997803015723824,
    0.0021459677437189061793, 0.0027887987935740759640, 0.0034602647778369039855, 0.0041572951208337952532,
    0.0048776559835423925804, 0.0056196422072054831710, 0.0063819059373191794422, 0.0071633531836349841425,
    0.0079630774380170392396, 0.0087803149858089752347, 0.0096144136425022094101, 0.010464810181029979488,
    0.011331013597834597488, 0.012212592426255380661, 0.013109164931254991070, 0.014020391403181937334,
    0.014945968011691148079, 0.015885621839973162490, 0.016839106826039946359, 0.017806200410911360563,
    0.018786700744696029497, 0.019780424338009741737, 0.020787204072578117603, 0.021806887504283582125,
    0.022839335406385238829, 0.023884420511558170348, 0.024942026419731782971, 0.026012046645134218076,
    0.027094383780955798424, 0.028188948763978634421, 0.029295660224637394015, 0.030414443910466605492,
    0.031545232172893605499, 0.032687963508959533317, 0.033842582150874329031, 0.035009037697397411067,
    0.036187284781931419754, 0.037377282772959360128, 0.038578995503074859626, 0.039792391023374122670,
    0.041017441380414820816, 0.042254122413316231413, 0.043502413568888183301, 0.044762297732943280694,
    0.046033761076175166762, 0.047316792913181548703, 0.048611385573379494401, 0.049917534282706374944,
    0.051235237055126279830, 0.052564494593071689595, 0.053905310196046085104, 0.055257689676697038322,
    0.056621641283742874438, 0.057997175631200659098, 0.059384305633420264487, 0.060783046445479636051,
    0.062193415408540996150, 0.063615431999807331076, 0.065049117786753755036, 0.066494496385339779043,
    0.067951593421936607770, 0.069420436498728751675, 0.070901055162371828426, 0.072393480875708743023,
    0.073897746992364746308, 0.075413888734058408453, 0.076941943170480510100, 0.078481949201606426042,
    0.080033947542319910023, 0.081597980709237420930, 0.083174093009632380354, 0.084762330532368125386,
    0.086362741140756912277, 0.087975374467270219300, 0.089600281910032864534, 0.091237516631040162057,
    0.092887133556043546523, 0.094549189376055853718, 0.096223742550432800103, 0.097910853311492199618,
    0.099610583670637128826, 0.10132299742595363588, 0.10304816017125771553, 0.10478613930657016928,
    0.10653700405000166218, 0.10830082545103379867, 0.11007767640518539026, 0.11186763167005629731,
    0.11367076788274431301, 0.11548716357863353664, 0.11731689921155557057, 0.11916005717532768467,
    0.12101672182667483729, 0.12288697950954513498, 0.12477091858083096578, 0.12666862943751066518,
    0.12858020454522817870, 0.13050573846833078225, 0.13244532790138752023, 0.13439907170221363078,
    0.13636707092642885841, 0.13834942886358021406, 0.14034625107486244210, 0.14235764543247220043,
    0.14438372216063476473, 0.14642459387834493787, 0.14848037564386679222, 0.15055118500103990354,
    0.15263714202744286154, 0.15473836938446807312, 0.15685499236936522013, 0.15898713896931420572,
    0.16113493991759203183, 0.16329852875190180795, 0.16547804187493600915, 0.16767361861725019322,
    0.16988540130252766513, 0.17211353531532005700, 0.17435816917135348788, 0.17661945459049489581,
    0.17889754657247831241, 0.18119260347549629488, 0.18350478709776746150, 0.18583426276219711495,
    0.18818119940425430485, 0.19054576966319540013, 0.19292814997677133873, 0.19532852067956322315,
    0.19774706610509886464, 0.20018397469191127727, 0.20263943909370901930, 0.20511365629383770880,
    0.20760682772422204205, 0.21011915938898825914, 0.21265086199297827522, 0.21520215107537867786,
    0.21777324714870053264, 0.22036437584335949720, 0.22297576805812018050, 0.22560766011668406495,
    0.22826029393071670664, 0.23093391716962742173, 0.23362878343743333945, 0.23634515245705964715,
    0.23908329026244917002, 0.24184346939887722761, 0.24462596913189210901, 0.24743107566532763894,
    0.25025908236886230967, 0.25311029001562948171, 0.25598500703041538015, 0.25888354974901621678,
    0.26180624268936295243, 0.26475341883506220209, 0.26772541993204481808, 0.27072259679906003167,
    0.27374530965280298302, 0.27679392844851734458, 0.27986883323697289920, 0.28297041453878076010,
    0.28609907373707684673, 0.28925522348967773308, 0.29243928816189258772, 0.29565170428126120948,
    0.29889292101558177099, 0.30216340067569352897, 0.30546361924459023541, 0.30879406693456016794,
    0.31215524877417956945, 0.31554768522712893632, 0.31897191284495723773, 0.32242848495608914289,
    0.32591797239355619822, 0.32944096426413633091, 0.33299806876180896713, 0.33658991402867758144,
    0.34021714906678004560, 0.34388044470450243010, 0.34758049462163698567, 0.35131801643748334681,
    0.35509375286678745925, 0.35890847294874976196, 0.36276297335481777335, 0.36665807978151414890,
    0.37059464843514599421, 0.37457356761590215193, 0.37859575940958081092, 0.38266218149600982112,
    0.38677382908413768115, 0.39093173698479710717, 0.39513698183329015336, 0.39939068447523107877,
    0.40369401253053026739, 0.40804818315203238238, 0.41245446599716116772, 0.41691418643300289465,
    0.42142872899761659635, 0.42599954114303435739, 0.43062813728845883923, 0.43531610321563659758,
    0.44006510084235387501, 0.44487687341454851593, 0.44975325116275498919, 0.45469615747461548049,
    0.45970761564213768669, 0.46478975625042618067, 0.46994482528395999841, 0.47517519303737738299,
    0.48048336393045423016, 0.48587198734188493564, 0.49134386959403255500, 0.49690198724154955294,
    0.50254950184134769289, 0.50828977641064283495, 0.51412639381474855788, 0.52006317736823356823,
    0.52610421398361972602, 0.53225388026304326945, 0.53851687200286186590, 0.54489823767243963663,
    0.55140341654064131685, 0.55803828226258748140, 0.56480919291240022434, 0.57172304866482579008,
    0.57878735860284503057, 0.58601031847726802755, 0.59340090169173341521, 0.60096896636523224742,
    0.60872538207962206507, 0.61668218091520762326, 0.62485273870366592605, 0.63325199421436607968,
    0.64189671642726607018, 0.65080583341457104881, 0.66000084107899974178, 0.66950631673192477684,
    0.67935057226476538741, 0.68956649611707798890, 0.70019265508278816709, 0.71127476080507597882,
    0.72286765959357200702, 0.73503809243142351530, 0.74786862198519510742, 0.76146338884989624862,
    0.77595685204011559675, 0.79152763697249565519, 0.80842165152300838005, 0.82699329664305033399,
    0.84778550062398962096, 0.87170433238120363669, 0.90046992992574643800, 0.93814368086217467916,
    1
};

template<class RealType = double>
struct unit_exponential_distribution
{
    template<class Engine>
    RealType operator()(Engine& eng) {
        const double * const table_x = exponential_table<double>::table_x;
        const double * const table_y = exponential_table<double>::table_y;
        RealType shift(0);
        for(;;) {
            std::pair<RealType, int> vals = generate_int_float_pair<RealType, 8>(eng);
            int i = vals.second;
            RealType x = vals.first * RealType(table_x[i]);
            if(x < RealType(table_x[i + 1])) return shift + x;
            // For i=0 we need to generate from the tail, but because this is an exponential
            // distribution, the tail looks exactly like the body, so we can simply repeat with a
            // shift:
            if (i == 0) shift += RealType(table_x[1]);
            else {
                RealType y01 = uniform_01<RealType>()(eng);
                RealType y = RealType(table_y[i]) + y01 * RealType(table_y[i+1] - table_y[i]);

                // All we care about is whether these are < or > 0; these values are equal to
                // (lbound) or proportional to (ubound) `y` minus the lower/upper bound.
                RealType y_above_ubound = RealType(table_x[i] - table_x[i+1]) * y01 - (RealType(table_x[i]) - x),
                         y_above_lbound = y - (RealType(table_y[i+1]) + (RealType(table_x[i+1]) - x) * RealType(table_y[i+1]));

                if (y_above_ubound < 0 // if above the upper bound reject immediately
                        &&
                        (
                         y_above_lbound < 0 // If below the lower bound accept immediately
                         ||
                         y < f(x) // Otherwise it's between the bounds and we need a full check
                        )
                   ) {
                    return x + shift;
                }
            }
        }
    }
    static RealType f(RealType x) {
        using std::exp;
        return exp(-x);
    }
};

} // namespace detail


/**
 * The exponential distribution is a model of \random_distribution with
 * a single parameter lambda.
 *
 * It has \f$\displaystyle p(x) = \lambda e^{-\lambda x}\f$
 *
 * The implementation uses the "ziggurat" algorithm, as described in
 *
 *  @blockquote
 *  "The Ziggurat Method for Generating Random Variables",
 *  George Marsaglia and Wai Wan Tsang, Journal of Statistical Software
 *  Volume 5, Number 8 (2000), 1-7.
 *  @endblockquote
 */
template<class RealType = double>
class exponential_distribution
{
public:
    typedef RealType input_type;
    typedef RealType result_type;

    class param_type
    {
    public:

        typedef exponential_distribution distribution_type;

        /**
         * Constructs parameters with a given lambda.
         *
         * Requires: lambda > 0
         */
        param_type(RealType lambda_arg = RealType(1.0))
          : _lambda(lambda_arg) { BOOST_ASSERT(_lambda > RealType(0)); }

        /** Returns the lambda parameter of the distribution. */
        RealType lambda() const { return _lambda; }

        /** Writes the parameters to a @c std::ostream. */
        BOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, param_type, parm)
        {
            os << parm._lambda;
            return os;
        }
        
        /** Reads the parameters from a @c std::istream. */
        BOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, param_type, parm)
        {
            is >> parm._lambda;
            return is;
        }

        /** Returns true if the two sets of parameters are equal. */
        BOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(param_type, lhs, rhs)
        { return lhs._lambda == rhs._lambda; }

        /** Returns true if the two sets of parameters are different. */
        BOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(param_type)

    private:
        RealType _lambda;
    };

    /**
     * Constructs an exponential_distribution with a given lambda.
     *
     * Requires: lambda > 0
     */
    explicit exponential_distribution(RealType lambda_arg = RealType(1.0))
      : _lambda(lambda_arg) { BOOST_ASSERT(_lambda > RealType(0)); }

    /**
     * Constructs an exponential_distribution from its parameters
     */
    explicit exponential_distribution(const param_type& parm)
      : _lambda(parm.lambda()) {}

    // compiler-generated copy ctor and assignment operator are fine

    /** Returns the lambda parameter of the distribution. */
    RealType lambda() const { return _lambda; }

    /** Returns the smallest value that the distribution can produce. */
    RealType min BOOST_PREVENT_MACRO_SUBSTITUTION () const
    { return RealType(0); }
    /** Returns the largest value that the distribution can produce. */
    RealType max BOOST_PREVENT_MACRO_SUBSTITUTION () const
    { return (std::numeric_limits<RealType>::infinity)(); }

    /** Returns the parameters of the distribution. */
    param_type param() const { return param_type(_lambda); }
    /** Sets the parameters of the distribution. */
    void param(const param_type& parm) { _lambda = parm.lambda(); }

    /**
     * Effects: Subsequent uses of the distribution do not depend
     * on values produced by any engine prior to invoking reset.
     */
    void reset() { }

    /**
     * Returns a random variate distributed according to the
     * exponential distribution.
     */
    template<class Engine>
    result_type operator()(Engine& eng) const
    { 
        detail::unit_exponential_distribution<RealType> impl;
        return impl(eng) / _lambda;
    }

    /**
     * Returns a random variate distributed according to the exponential
     * distribution with parameters specified by param.
     */
    template<class Engine>
    result_type operator()(Engine& eng, const param_type& parm) const
    { 
        return exponential_distribution(parm)(eng);
    }

    /** Writes the distribution to a std::ostream. */
    BOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, exponential_distribution, ed)
    {
        os << ed._lambda;
        return os;
    }

    /** Reads the distribution from a std::istream. */
    BOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, exponential_distribution, ed)
    {
        is >> ed._lambda;
        return is;
    }

    /**
     * Returns true iff the two distributions will produce identical
     * sequences of values given equal generators.
     */
    BOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(exponential_distribution, lhs, rhs)
    { return lhs._lambda == rhs._lambda; }
    
    /**
     * Returns true iff the two distributions will produce different
     * sequences of values given equal generators.
     */
    BOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(exponential_distribution)

private:
    result_type _lambda;
};

} // namespace random

using random::exponential_distribution;

} // namespace boost

#endif // BOOST_RANDOM_EXPONENTIAL_DISTRIBUTION_HPP
