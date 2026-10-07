[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · **Tiếng Việt** · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Tổng quan

- **Định nghĩa vector thưa**

	Vector thưa không phải là vector trong toán học, mà đúng hơn là giống với cách bạn thực sự viết một con số; “chỉ số” của nó trải dài từ dương vô cực xuống âm vô cực. Tất nhiên ngôn ngữ máy tính không có vô cực, nên trong TypeScript nó là `number`, trong C++ là `int64_t`, còn trong Rust là `i64`.

	Và đã không phải là vector thì đương nhiên nó cũng không làm được phép toán đại số nào.

- **Nguyên lý của vector thưa**

	Bạn hỏi giá trị ở bất kỳ trọng số vị trí nào cũng đều nhận được câu trả lời — vậy điều đó được thực hiện bằng cách nào?

	Ta chỉ lưu những vị trí khác với giá trị mặc định, rồi lưu riêng thêm một giá trị mặc định. Khi bạn tra những vị trí không được lưu, vector đưa cho bạn giá trị mặc định.

## Thư viện

| Ngôn ngữ | Gói | Phiên bản | Trạng thái | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | đã phát hành | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | đã phát hành | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | đã phát hành | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*chưa có trên vcpkg hay Conan*

Chỉ cần trỏ CMake vào kho mã nguồn

```cmake
include(FetchContent)

FetchContent_Declare(sparse-vector
  GIT_REPOSITORY https://github.com/Calbona/sparse-vector
  GIT_TAG main
  SOURCE_SUBDIR c++
)
FetchContent_MakeAvailable(sparse-vector)

target_link_libraries(your-target PRIVATE Calbona::sparse-vector)
```

Một bản checkout đặt cạnh dự án của bạn cũng chạy y như vậy, với `add_subdirectory(path/to/sparse-vector/c++)`
Một tiền tố cài đặt cũng xuất ra gói CMake, nên `find_package(sparse-vector)` cũng dùng được

### Rust

```sh
cargo add sparse-vector-rs
```

## Chi tiết ngữ nghĩa

### Vector

- Không phải vector trong toán học, cũng không phải mảng trong máy tính, mà là cấu trúc dữ liệu đặc biệt do thư viện này cung cấp.

### Thưa

- Nghĩa là sức chứa của vector lớn hơn hẳn số phần tử của nó, có những trọng số vị trí chưa được ghi dữ liệu vào một cách tường minh.

### Chỉ số

- Chỉ số của vector là toàn bộ số nguyên, có thể dương hoặc âm; nó biểu thị khái niệm kiểu như hàng đơn vị, hàng chục.

### Giá trị

- Thứ mà ta thực sự muốn lưu, ví như chữ số ở hàng trăm hay hàng nghìn — chỉ có điều kiểu của nó không nhất thiết là số, mà có thể là bất cứ thứ gì.

### Phần tử

- Một chỉ số cộng với một giá trị tạo thành một đối tượng gọi là phần tử; đó chính là những gì vector thực sự lưu.

### Giá trị mặc định

- Những chỗ trong vector thưa không được ghi dữ liệu vào một cách tường minh thì mang giá trị mặc định; bạn có thể ví với những số 0 không viết ra khi viết một con số — ta thường viết 1 chứ không viết 0001.000, đúng không?

- Dựng xong vector rồi, giá trị mặc định vẫn có thể thay thế được. Lạ thật, chẳng biết đem ra dùng để làm gì, nhưng tôi để sẵn khả năng đó cho bạn.

### Tự động giữ bộ nhớ tối thiểu

- Thay giá trị mặc định thì lập tức xoá những phần tử có giá trị bằng nó.

- Thay cách so sánh bằng nhau thì cũng vậy.

- Ghi vào một trọng số vị trí một giá trị vốn là giá trị mặc định thì cũng như xoá thứ đang có ở đó.

### Sự bằng nhau

- Giá trị có bằng giá trị mặc định không? Theo cách so sánh thường ngày của từng ngôn ngữ.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- Bạn cũng có thể đưa cho vector một phép so sánh riêng: nó nhận hai tham số là giá trị cần so sánh và giá trị mặc định hiện tại, rồi trả về kiểu boolean, thay cho cách so sánh thường ngày.

- So sánh hai vector là hai việc có tên riêng, khác hẳn với việc loại bỏ: `isEqualTo` / `is_equal_to` xác định xem hai vector có bằng nhau không, còn `differences` liệt kê những phần tử mà đối tượng nhận khác với đối tượng kia. Cả hai đều **lấy phép so sánh của đối tượng nhận làm chuẩn**, nên khi hai bên có phép so sánh khác nhau, `a.isEqualTo(b)` và `b.isEqualTo(a)` có thể cho ra những câu trả lời khác nhau.

- Xin lưu ý rằng cách so sánh bằng nhau của mỗi ngôn ngữ có vài trường hợp có thể đi ngược với trực giác.
	- `NaN` không bằng chính nó.
	- `0`, `'0'`, `false` và `null` thuộc bốn kiểu khác nhau, nên không bằng nhau.
	- `===` so sánh tham chiếu của đối tượng, còn `operator==` và `PartialEq` so sánh cấu trúc.

## API

| Công dụng | TypeScript | C++ | Rust | Kiểu trả về |
| --- | --- | --- | --- | --- |
| Tạo vector thưa (bỏ qua giá trị mặc định) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | Vector mới |
| Tạo vector thưa | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | Vector mới |
| Lấy giá trị mặc định | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts giá trị, cpp, rust tham chiếu |
| Đổi giá trị mặc định | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts không, cpp, rust kiểu boolean |
| Lấy phép đánh giá bằng nhau | `getEquality` | `get_equality()` | `get_equality()` | ts phép đánh giá hoặc `undefined`, cpp, rust phép đánh giá hoặc rỗng |
| Đổi phép đánh giá bằng nhau | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts không, cpp, rust kiểu boolean |
| Lấy số phần tử | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | Số nguyên |
| Lấy chiều có nghĩa | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | Số nguyên |
| Lấy chiều dương | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | Số nguyên |
| Lấy chiều âm | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | Số nguyên |
| Lấy giá trị tại chỉ số | `get(index)` | `get(index)` | `get(index)` | Kiểu của giá trị |
| Ghi giá trị tại chỉ số | `set(index, value)` | `set(index, value)` | `set(index, value)` | Chính vector |
| Đặt lại giá trị tại chỉ số | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | Kiểu boolean |
| Đặt lại toàn bộ vector, không đặt lại giá trị mặc định | `resetVector()` | `reset_vector()` | `reset_vector()` | Kiểu boolean |
| Mọi phần tử, chỉ số giảm dần | `elements()` | `elements()` | `elements()` | Mảng phần tử |
| Mọi phần tử, chỉ số tăng dần | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | Mảng phần tử |
| Mọi chỉ số, giảm dần | `indexes()` | `indexes()` | `indexes()` | Mảng chỉ số |
| Mọi chỉ số, tăng dần | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | Mảng chỉ số |
| Mọi giá trị, chỉ số giảm dần | `values()` | `values()` | `values()` | Mảng giá trị |
| Mọi giá trị, chỉ số tăng dần | `invertedValues()` | `inverted_values()` | `inverted_values()` | Mảng giá trị |
| Phần tử thứ (n+1) từ vị trí cao | `element(n)` | `element(n)` | `element(n)` | Phần tử |
| Chỉ số của phần tử thứ (n+1) từ vị trí cao | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | Chỉ số |
| Giá trị của phần tử thứ (n+1) từ vị trí cao | `elementValue(n)` | `element_value(n)` | `element_value(n)` | Giá trị |
| Phần tử thứ (n+1) từ vị trí thấp | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | Phần tử |
| Chỉ số của phần tử thứ (n+1) từ vị trí thấp | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | Chỉ số |
| Giá trị của phần tử thứ (n+1) từ vị trí thấp | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | Giá trị |
| Chữ số có nghĩa thứ (n+1) từ vị trí cao | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | Giá trị |
| Chữ số có nghĩa thứ (n+1) từ vị trí thấp | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | Giá trị |
| Duyệt | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | Trình lặp (cho mượn phần tử theo chỉ số giảm dần) |
| Sao chép | `clone()` | hàm dựng sao chép | `clone()` | Vector mới |
| Tạo hàng loạt | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | Vector mới |
| Xác định hai vector có bằng nhau không | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | Kiểu boolean |
| Liệt kê những phần tử khác với vector kia | `differences(other)` | `differences(other)` | `differences(other)` | Mảng phần tử |

*Trong Rust, việc lấy giá trị và liệt kê đòi hỏi `T: Clone`, còn ghi, đặt lại và đổi giá trị mặc định đòi hỏi `T: PartialEq`; xác định hai vector có bằng nhau cũng đòi hỏi `T: PartialEq`, và tìm khác biệt thì cần thêm `T: Clone`; trong C++, tương ứng cần khả năng sao chép và `operator==`*

*Điều kiện tiên quyết để tìm khác biệt là hai giá trị mặc định cùng kiểu và bằng nhau theo phép so sánh của đối tượng nhận; nếu không thoả, TypeScript ném `TypeError`, C++ ném `std::invalid_argument`, còn Rust panic*

*Khi vượt ngoài phạm vi, hoặc khi đo một chữ số có nghĩa trên vector rỗng, TypeScript ném `TypeError` / `RangeError`, C++ ném `std::out_of_range`, còn Rust panic*

## Giấy phép

MIT
