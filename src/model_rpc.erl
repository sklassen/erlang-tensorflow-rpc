-module(model_rpc).
-export([start/0, start/1, stop/0]).
-export([scale/1, shift/1,scale_and_shift/2]).
-define(TIMEOUT,3000).

start([]) -> start().
start() ->
    register(
        ?MODULE,
        spawn(fun() ->
            process_flag(trap_exit, true),
            Port = open_port({spawn, "./priv/model"}, [{packet, 2}]),
            loop(Port)
        end)
    ).

stop() ->
    ?MODULE ! stop.
scale_and_shift(X, Y) -> call_port({scale_and_shift, X, Y}).
shift(X) -> call_port({shift, X}).
scale(X) -> call_port({scale, X}).
call_port(Msg) ->
    ?MODULE ! {call, self(), Msg},
    receive
        {?MODULE, Result} ->
            Result
    after ?TIMEOUT
        -> erlang:error(timeout)
    end.

loop(Port) ->
    receive
        {call, Caller, Msg} ->
            Port ! {self(), {command, encode(Msg)}},
            receive
                {Port, {data, Data}} ->
                    Caller ! {?MODULE, decode(Data)}
            end,
            loop(Port);
        stop ->
            Port ! {self(), close},
            receive
                {Port, closed} ->
                    exit(normal)
            end;
        {'EXIT', Port, Reason} ->
            exit({port_terminated, Reason})
    end.

encode({scale_and_shift, X, Y}) -> [0, X, Y];
encode({scale, X}) -> [1, X];
encode({shift, X}) -> [2, X].

decode([Int]) -> Int.
