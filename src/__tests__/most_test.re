open Jest;

module Helpers = {
  let noOp _ => ();
  let combineArray coll el => Js.Array.push el coll |> noOp;
  let combineList collRef el => (collRef := [el, ...!collRef]) |> noOp;
  let asyncExpectToEqual expected actual =>
    Js.Promise.resolve Expect.(expect actual |> toEqual expected);
};


/**
 * Stream creation
 **/
testPromise
  "empty"
  (
    fun _ => {
      let empty = Most.empty ();
      let success _ => Helpers.asyncExpectToEqual true true;
      Most.observe (fun _ => raise (Failure "this shouldn't happen")) empty |>
      Js.Promise.then_ success
    }
  );

testAsync
  "just"
  (
    fun done_ => {
      open Expect;
      let success result => done_ (expect result |> toBe "asdf");
      Most.just "asdf" |> Most.observe success |> Helpers.noOp
    }
  );

testPromise
  "from"
  (
    fun _ => {
      let result = [||];
      let combine = Helpers.combineArray result;
      let success _ => Helpers.asyncExpectToEqual [|1, 2|] result;
      Most.from [|1, 2|] |> Most.observe combine |> Js.Promise.then_ success
    }
  );

testPromise
  "fromPromise"
  (
    fun _ => {
      let promise = Js.Promise.make (fun ::resolve reject::_ => resolve "OK" [@bs]);
      let result = ref "";
      let success = Helpers.asyncExpectToEqual "OK";
      Most.fromPromise promise |> Most.observe (fun res => result := res) |>
      Js.Promise.then_ (fun _ => success !result)
    }
  );

testPromise "periodic";

testPromise "never";

testPromise "iterate";

testPromise "iteratePromise";

testPromise "fromEvent";

testPromise
  "concat"
  (
    fun _ => {
      let result = [||];
      let combine = Helpers.combineArray result;
      let s1 = Most.from [|1, 2, 3|];
      let s2 = Most.from [|4, 5, 6|];
      let success _ => Helpers.asyncExpectToEqual [|1, 2, 3, 4, 5, 6|] result;
      Most.concat s2 s1 |> Most.observe combine |> Js.Promise.then_ success
    }
  );

testPromise
  "startWith"
  (
    fun _ => {
      let result = [||];
      let combine = Helpers.combineArray result;
      let s = Most.from [|1, 2, 3|];
      let success _ => Helpers.asyncExpectToEqual [|0, 1, 2, 3|] result;
      Most.startWith 0 s |> Most.observe combine |> Js.Promise.then_ success
    }
  );

testPromise
  "unfold"
  (
    fun _ => {
      let result = ref [];
      let combine el => (result := [el, ...!result]) |> Helpers.noOp;
      let success _ => Helpers.asyncExpectToEqual [3, 2, 1] !result;
      Most.unfold
        (
          fun prev =>
            if (prev < 3) {
              Some (prev + 1, prev + 1)
            } else {
              None
            }
        )
        0 |>
      Most.observe combine |>
      Js.Promise.then_ success
    }
  );

testPromise
  "fromList"
  (
    fun _ => {
      let result = ref [];
      let combine = Helpers.combineList result;
      /* combineList adds them to the list in reverse order */
      let success _ => Helpers.asyncExpectToEqual [3, 2, 1] !result;
      Most.fromList [1, 2, 3] |> Most.observe combine |> Js.Promise.then_ success
    }
  );


/**
 * Error handling
 **/
testPromise "recoverWith";

testPromise "throwError";


/**
 * Transforming
 **/
testPromise
  "map"
  (
    fun _ => {
      let result = [||];
      let combineResult = Helpers.combineArray result;
      let success _ => Helpers.asyncExpectToEqual [|2, 4, 6|] result;
      Most.(
        from [|1, 2, 3|] |> map (fun n => n * 2) |> observe combineResult |>
        Js.Promise.then_ success
      )
    }
  );

testPromise
  "constant"
  (
    fun _ => {
      let result = [||];
      let combine = Helpers.combineArray result;
      let success _ => Helpers.asyncExpectToEqual [|10, 10, 10|] result;
      Most.from [|1, 2, 3|] |> Most.constant 10 |> Most.observe combine |> Js.Promise.then_ success
    }
  );

testPromise
  "scan"
  (
    fun _ => {
      let result = [||];
      let combine = Helpers.combineArray result;
      let success _ => Helpers.asyncExpectToEqual [|0, 1, 3, 6, 10|] result;
      Most.from [|1, 2, 3, 4|] |> Most.scan (fun accum n => accum + n) 0 |> Most.observe combine |>
      Js.Promise.then_ success
    }
  );

testPromise
  "flatMap"
  (
    fun _ => {
      let result = [||];
      let combine = Helpers.combineArray result;
      let success _ => Helpers.asyncExpectToEqual [|0, 1, 0, 2, 0, 3, 0, 4|] result;
      Most.from [|1, 2, 3, 4|] |> Most.flatMap (fun x => Most.from [|0, x|]) |>
      Most.observe combine |>
      Js.Promise.then_ success
    }
  );

testPromise "continueWith";

testPromise "concatMap";

testPromise "ap";

testPromise "timestamp";

testPromise "tap";


/**
 * Filtering
 **/
testPromise "filter";

testPromise "skipRepeats";

testPromise "skipRepeatsWith";


/**
 * Slicing
 **/
testPromise "slice";

testPromise "take";

testPromise "skip";


/**
 * Combining
 **/
testPromise "merge";

testPromise "combine";

testPromise "zip";
