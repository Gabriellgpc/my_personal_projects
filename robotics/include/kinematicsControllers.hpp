#pragma once
#include "pid.hpp"
#include <list>
#include <configSpaceTools.hpp>  //Config

// Seguidor de Caminho [Samson]
// Descrição do robô em relação ao caminho:
// – baseada em um referencial Serret-Frenet
// – vetor tangente, vetor normal e vetor binormal
// – O referencial S-F é localizado no ponto da
// projeção ortonormal do robô sobre o caminho.
// – O referencial S-F move-se ao longo do
// caminho.
class PathFollowController
{
public:
    PathFollowController();
    PathFollowController(const double K_ang, const double K_lin, const std::list<Config> &points);
    PathFollowController(const double K_ang, const double K_lin);
    ~PathFollowController();

    bool step(const Config &curr_q,
              double &v, double &w,
              Config &ref_q,
              double &lin_error, double &ang_error);
    //update the  parameters
    void update(const double K_ang, const double K_lin, const std::list<Config> &points);
    void closestPoint(const Config &q, Config &ref, double &mindist, double &kappa);

private:
    double _K_ang, _K_lin; //ganho linear e ganho linear respectivamente
    std::list<Config> _points;
    std::list<Config>::iterator _prev_point;
};

class PositionController
{
public:
    PositionController(const double lin_Kp, const double lin_Ki, const double lin_Kd,
                       const double ang_Kp, const double ang_Ki, const double ang_Kd);

    /* input: (referencial global)
    * x_ref:    coordenada x da posição de referência [m]
    * y_ref:    coordenada y da posição de referência [m]
    * x_curr:   coordenada x atual [m]
    * y_curr:   coordenada y atual [m]
    * th_curr:  orientação atual no plano x-y [rad]
    *output: 
    * u_v:      velocidade linear [m/s]
    * u_w:      velocidade angular[rad/s] 
    * lin_error:  erro linear [m]  (util para debug) 
    * ang_error:  erro angular[rad](util para debug) 
    * return: true ao chegar na referencia, false caso contrario
    */
    bool step(const Config &q_ref, const Config &q_curr);
    //reset to PID controllers
    void reset();

    //update the PID's parameters
    void update(const double lin_Kp, const double lin_Ki, const double lin_Kd,
                const double ang_Kp, const double ang_Ki, const double ang_Kd);
    inline double get_v()const{ return prev_v; }
    inline double get_w()const{ return prev_w; }
    inline double get_lin_error()const{ return prev_lin_error; }
    inline double get_ang_error()const{ return prev_ang_error; }
private:
    PID lin_controller;
    PID ang_controller;

    double prev_v;
    double prev_w;
    double prev_lin_error;
    double prev_ang_error;
};

// DFL - Dynamic Feedback Linearization (Novel, et al 1995)
// envolve Realimentação PD e Compensação do Modelo
// Não Linear.
class TrajController
{
public:
    enum TRAJ_TYPE
    {
        POLY3 = 0,
        CIRCLE = 1
    };

    TrajController(const double _Kd, const double _Kp);
    ~TrajController() { reset(); }
    void setTrajectory(const double pathCoef[], const double vmax);
    /*
    * input: 
    * currConfig[3] : {x[m], y[m], theta[rad]}, configuração atual do robo
    * currVelocity  : velocidade linear do robo com relação ao mundo [m/s]
    * output:
    * v : velocidade linear que deve ser aplicada no robo [m/s]
    * w : velocidade angular que deve ser aplicada no robo [rad/s]
    */
    bool step(const double currConfig[],
              double &x, double &y, double &v_l, double &dv_l, double &wc,
              double &v, double &w);

    //controller reset
    void reset();

    // Perfil de Velocidade Cosenoidal
    // v(t) = [1 – cos(2pi*t/tmax )].vmax/2
    double speedProfile_cos(const double t, const double tmax, const double vmax);

    double speedProfile_cos_derivate(const double t, const double tmax, const double vmax);

private:
    double Kd, Kp;
    bool inited = false;
    bool haveTraj = false;
    double coef[8]; //parametros que definem o caminho (polinomio de grau 3)
    double vmax;
};