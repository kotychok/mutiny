module CreatableWithDSL
  def context_class
    @context_class ||= Class.new {
      def initialize(record)
        @record = record
      end
    }
  end

  def dsl(attribute, type)
    method_name = attribute

    case type
    when :string
      method_callable = ->(value) {
        @record.send "#{method_name}=", value
        @record
      }
    when :boolean
      method_callable = -> {
        @record.send "#{method_name}=", true
        @record
      }
    when :array
      method_callable = ->(*value) {
        @record.send "#{method_name}=", value
        @record
      }
    end

    context_class.class_eval do
      define_method method_name, &method_callable
    end
  end
end
