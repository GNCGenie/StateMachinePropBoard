#ifndef TINYFSM_HPP_INCLUDED
#define TINYFSM_HPP_INCLUDED

#ifndef TINYFSM_NOSTDLIB
#include <type_traits>
#endif

namespace tinyfsm
{

  // --------------------------------------------------------------------------

  struct Event { };

  // --------------------------------------------------------------------------

  template<typename S>
  struct _state_instance
  {
    using value_type = S;
    using type = _state_instance<S>;
    static S value;
  };

  template<typename S>
  typename _state_instance<S>::value_type _state_instance<S>::value;

  // --------------------------------------------------------------------------

  template<typename F>
  class Fsm
  {
  public:

    using fsmtype = Fsm<F>;
    using state_ptr_t = F *;

    static state_ptr_t current_state_ptr;

    // public, leaving ability to access state instance (e.g. on reset)
    template<typename S>
    static constexpr S & state(void) {
      static_assert(is_same_fsm<F, S>::value, "accessing state of different state machine");
      return _state_instance<S>::value;
    }

    template<typename S>
    static constexpr bool is_in_state(void) {
      static_assert(is_same_fsm<F, S>::value, "accessing state of different state machine");
      return current_state_ptr == &_state_instance<S>::value;
    }

  /// state machine functions
  public:

    // explicitely specialized in FSM_INITIAL_STATE macro
    static void set_initial_state();

    static void reset() { };

    static void enter() {
      current_state_ptr->entry();
    }

    static void start() {
      set_initial_state();
      enter();
    }

    template<typename E>
    static void dispatch(E const & event) {
      current_state_ptr->react(event);
    }


  /// state transition functions
  protected:

    template<typename S>
    void transit(void) {
      static_assert(is_same_fsm<F, S>::value, "transit to different state machine");
      current_state_ptr->exit();
      current_state_ptr = &_state_instance<S>::value;
      current_state_ptr->entry();
    }

    template<typename S, typename ActionFunction>
    void transit(ActionFunction action_function) {
      static_assert(is_same_fsm<F, S>::value, "transit to different state machine");
      current_state_ptr->exit();
      // NOTE: do not send events in action_function definisions.
      action_function();
      current_state_ptr = &_state_instance<S>::value;
      current_state_ptr->entry();
    }

    template<typename S, typename ActionFunction, typename ConditionFunction>
    void transit(ActionFunction action_function, ConditionFunction condition_function) {
      if(condition_function()) {
        transit<S>(action_function);
      }
    }
  };

  template<typename F>
  typename Fsm<F>::state_ptr_t Fsm<F>::current_state_ptr;

  // --------------------------------------------------------------------------

  template<typename... FF>
  struct FsmList;

  template<> struct FsmList<> {
    static void set_initial_state() { }
    static void reset() { }
    static void enter() { }
    template<typename E>
    static void dispatch(E const &) { }
  };

  template<typename F, typename... FF>
  struct FsmList<F, FF...>
  {
    using fsmtype = Fsm<F>;

    static void set_initial_state() {
      fsmtype::set_initial_state();
      FsmList<FF...>::set_initial_state();
    }

    static void reset() {
      F::reset();
      FsmList<FF...>::reset();
    }

    static void enter() {
      fsmtype::enter();
      FsmList<FF...>::enter();
    }

    static void start() {
      set_initial_state();
      enter();
    }

    template<typename E>
    static void dispatch(E const & event) {
      fsmtype::template dispatch<E>(event);
      FsmList<FF...>::template dispatch<E>(event);
    }
  };

  // --------------------------------------------------------------------------

  template<typename... SS> struct StateList;
  template<> struct StateList<> {
    static void reset() { }
  };
  template<typename S, typename... SS>
  struct StateList<S, SS...>
  {
    static void reset() {
      _state_instance<S>::value = S();
      StateList<SS...>::reset();
    }
  };

  // --------------------------------------------------------------------------

  template<typename F>
  struct MooreMachine : tinyfsm::Fsm<F>
  {
    virtual void entry(void) { };  /* entry actions in some states */
    void exit(void) { };           /* no exit actions */
  };

  template<typename F>
  struct MealyMachine : tinyfsm::Fsm<F>
  {
    // input actions are modeled in react():
    // - conditional dependent of event type or payload
    // - transit<>(ActionFunction)
    void entry(void) { };  /* no entry actions */
    void exit(void) { };   /* no exit actions */
  };

} /* namespace tinyfsm */


#define FSM_INITIAL_STATE(_FSM, _STATE)                               \
namespace tinyfsm {                                                   \
  template<> void Fsm< _FSM >::set_initial_state(void) {              \
    current_state_ptr = &_state_instance< _STATE >::value;            \
  }                                                                   \
}

#endif /* TINYFSM_HPP_INCLUDED */
